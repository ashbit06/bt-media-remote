#include <HijelHID_BLEKeyboard.h>
#include <string.h>

HijelHID_BLEKeyboard keyboard("bt-remote", "group 4", 100);

enum {
  PLAY_PAUSE = 2,
  SKIP_FORWARD = 3,
  SKIP_BACKWARD = 4,
  VOLUME_UP = 5,
  VOLUME_DOWN = 6
};


// extra variable for detecting the moment when we are connected
int connected = 0;

// keep track of what buttons are currently held
int playPauseButton, skipForwardButton, skipBackwardButton, volumeUpButton, volumeDownButton = 0;

// keep track of what buttons were pressed in the previous loop
int playPauseButtonLast, skipForwardButtonLast, skipBackwardButtonLast, volumeUpButtonLast, volumeDownButtonLast = 0;

// keep track of how long the buttons were pressed for (currently)
int playPauseButtonHeld, skipForwardButtonHeld, skipBackwardButtonHeld, volumeUpButtonHeld, volumeDownButtonHeld = 0;


void writeRGB(bool r, bool g, bool b) {
  digitalWrite(LEDR, !r);
  digitalWrite(LEDG, !g);
  digitalWrite(LEDB, !b);
}

void setup() {
  // initialize serial, disable when not connected to usb
  Serial.begin(115200);
  while (!Serial);
  
  // initialize inputs
  pinMode(PLAY_PAUSE, INPUT);
  pinMode(SKIP_FORWARD, INPUT);
  pinMode(SKIP_BACKWARD, INPUT);
  pinMode(VOLUME_UP, INPUT);
  pinMode(VOLUME_DOWN, INPUT);

  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  // initialize bluetooth keyboard
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

  if (playPauseButtonLast == playPauseButton) playPauseButtonHeld++;            else playPauseButtonHeld = 0;
  if (skipForwardButtonLast == skipForwardButton) skipForwardButtonHeld++;      else skipForwardButtonHeld = 0;
  if (skipBackwardButtonLast == skipBackwardButton) skipBackwardButtonHeld++;   else skipBackwardButtonHeld = 0;
  if (volumeUpButtonLast == volumeUpButton) volumeUpButtonHeld++;               else volumeUpButtonHeld = 0;
  if (volumeDownButtonLast == volumeDownButton) volumeDownButtonHeld++;         else volumeDownButtonHeld = 0;

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
    snprintf(buffer, sizeof(buffer), "%d %d %d %d %d", playPauseButton, skipBackwardButton, skipForwardButton, volumeUpButton, volumeDownButton);
    Serial.println("P < > + -");
    Serial.println(buffer);

    // pairing -- only play & volume up buttons pressed for 5 seconds
    if (playPauseButton && volumeUpButton && !(skipForwardButton || skipBackwardButton || volumeDownButton)) {
      Serial.println("pairing mode");
    } else {  // not pairing
      if (playPauseButton) keyboard.tap(MEDIA_PLAY_PAUSE);
      if (skipForwardButton) keyboard.tap(MEDIA_NEXT_TRACK);
      if (skipBackwardButton) keyboard.tap(MEDIA_PREV_TRACK);
      if (volumeUpButton) keyboard.tap(MEDIA_VOLUME_UP);
      if (volumeDownButton) keyboard.tap(MEDIA_VOLUME_DOWN);
    }
  }
  
  else {
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
