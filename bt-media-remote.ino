#include <HijelHID_BLEKeyboard.h>
#include <string.h>


HijelHID_BLEKeyboard keyboard("bt-remote", "group 4", 100);

enum {
  PLAY_PAUSE = 2,
  SKIP_FORWARD = 3,
  SKIP_BACKWARD = 4,
  VOLUME_UP = 5,
  VOLUME_DOWN = 6,
  BAT = A0,
  LED_R = A1,
  LED_G = A2,
  LED_B = A3
};


// extra variable for detecting the moment when we are connected
int connected = 0;

// keep track of what buttons are currently held
int playPauseButton, skipForwardButton, skipBackwardButton,
  volumeUpButton, volumeDownButton = 0;

// keep track of what buttons were pressed in the previous loop
int playPauseButtonLast, skipForwardButtonLast,
  skipBackwardButtonLast, volumeUpButtonLast, volumeDownButtonLast = 0;

// Track held duration in milliseconds, with a start timestamp for each press.
uint32_t playPauseButtonHeld, skipForwardButtonHeld,
  skipBackwardButtonHeld, volumeUpButtonHeld, volumeDownButtonHeld = 0;

uint32_t playPauseButtonPressedAt, skipForwardButtonPressedAt,
  skipBackwardButtonPressedAt, volumeUpButtonPressedAt,
  volumeDownButtonPressedAt = 0;


void writeRGB(bool r, bool g, bool b) {
  digitalWrite(LEDR, !r);
  digitalWrite(LEDG, !g);
  digitalWrite(LEDB, !b);
}

void updateButtonHeld(int button, int buttonLast, uint32_t now,
  uint32_t &pressedAt, uint32_t &held) {
  if (button) {
    if (!buttonLast) pressedAt = now;
    held = now - pressedAt;
  } else {
    pressedAt = 0;
    held = 0;
  }
}

void setup() {
  // initialize serial, disable when not connected to usb
  Serial.begin(115200);
  while (!Serial) {}

  // initialize pins
  pinMode(PLAY_PAUSE, INPUT_PULLDOWN);
  pinMode(SKIP_FORWARD, INPUT_PULLDOWN);
  pinMode(SKIP_BACKWARD, INPUT_PULLDOWN);
  pinMode(VOLUME_UP, INPUT_PULLDOWN);
  pinMode(VOLUME_DOWN, INPUT_PULLDOWN);
  pinMode(BAT, INPUT);

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  // initialize bluetooth keyboard
  keyboard.setTapDelay(50);
  keyboard.setKeyGap(50);
  keyboard.begin();
  Serial.println("bluetooth started");
  writeRGB(LOW, HIGH, LOW);
  delay(5000);
}

void loop() {
  playPauseButton = digitalRead(PLAY_PAUSE);
  skipForwardButton = digitalRead(SKIP_FORWARD);
  skipBackwardButton = digitalRead(SKIP_BACKWARD);
  volumeUpButton = digitalRead(VOLUME_UP);
  volumeDownButton = digitalRead(VOLUME_DOWN);

  uint32_t now = millis();
  updateButtonHeld(playPauseButton, playPauseButtonLast, now,
    playPauseButtonPressedAt, playPauseButtonHeld);
  updateButtonHeld(skipForwardButton, skipForwardButtonLast, now,
    skipForwardButtonPressedAt, skipForwardButtonHeld);
  updateButtonHeld(skipBackwardButton, skipBackwardButtonLast, now,
    skipBackwardButtonPressedAt, skipBackwardButtonHeld);
  updateButtonHeld(volumeUpButton, volumeUpButtonLast, now,
    volumeUpButtonPressedAt, volumeUpButtonHeld);
  updateButtonHeld(volumeDownButton, volumeDownButtonLast, now,
    volumeDownButtonPressedAt, volumeDownButtonHeld);

  if (keyboard.isConnected()) {
    if (!connected) {
      Serial.println("connected to device");
      for (int i = 0; i < 4; i++) {
        // blink 4 times in 1 second
        writeRGB(LOW, LOW, HIGH);
        delay(125);
        writeRGB(LOW, LOW, LOW);
        delay(125);
      }
      writeRGB(HIGH, HIGH, HIGH);
      connected = 1;
      delay(500);
    }

    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%d %d %d %d %d", playPauseButton,
      skipBackwardButton, skipForwardButton, volumeUpButton, volumeDownButton);
    Serial.println("P < > + -");
    Serial.println(buffer);

    // pairing -- only play & volume up buttons pressed for 5 seconds
    if (playPauseButton && volumeUpButton &&
        playPauseButtonHeld > 5000 && volumeUpButtonHeld > 5000 &&
        !(skipForwardButton || skipBackwardButton || volumeDownButton)) {
      Serial.println("start pairing mode");
      keyboard.clearBonds();
    // everything else -- tap once, if held longer than 750ms keep tapping
    } else if (playPauseButton) {                               // play/pause
      if (playPauseButtonHeld == 0) keyboard.tap(MEDIA_PLAY_PAUSE);
      else if (playPauseButtonHeld > 750)
        keyboard.tap(MEDIA_PLAY_PAUSE, 0, 200);
    } else if (skipForwardButton) {                             // skip forward
      if (skipForwardButtonHeld == 0) keyboard.tap(MEDIA_NEXT_TRACK);
      else if (skipForwardButtonHeld > 750)
        keyboard.tap(MEDIA_NEXT_TRACK, 0, 200);
    } else if (skipBackwardButton) {                            // skip backward
      if (skipBackwardButtonHeld == 0) keyboard.tap(MEDIA_PREV_TRACK);
      else if (skipBackwardButtonHeld > 750)
        keyboard.tap(MEDIA_PREV_TRACK, 0, 200);
    } else if (volumeUpButton) {                                // volume up
      if (volumeUpButtonHeld == 0) keyboard.tap(MEDIA_VOLUME_UP);
      else if (volumeUpButtonHeld > 750)
        keyboard.tap(MEDIA_VOLUME_UP, 0, 200);
    } else if (volumeDownButton) {                              // volume down
      if (volumeDownButtonHeld == 0) keyboard.tap(MEDIA_VOLUME_DOWN);
      else if (volumeDownButtonHeld > 750)
        keyboard.tap(MEDIA_VOLUME_DOWN, 0, 200);
    }

    // update battery level every 5 seconds
    if (now % 5000 == 0) {
      keyboard.setBatteryLevel(analogRead(BAT));
      // TODO(ashbit06): use an actual battery percentage calculation
    }
  } else {
    if (connected) {
      Serial.println("disconnected from device");
      connected = 0;
    }

    Serial.println("searching for bluetooth device...");

    // blink blue for every second that there is no device
    writeRGB(LOW, LOW, HIGH);
    delay(500);
    writeRGB(LOW, LOW, LOW);
    delay(500);
  }

  playPauseButtonLast = playPauseButton;
  skipForwardButtonLast = skipForwardButton;
  skipBackwardButtonLast = skipBackwardButton;
  volumeUpButtonLast = volumeUpButton;
  volumeDownButtonLast = volumeDownButton;

  keyboard.releaseAll();
}
