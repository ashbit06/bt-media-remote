#include <HijelHID_BLEKeyboard.h>

HijelHID_BLEKeyboard keyboard("bt-remote", "chuds", 100);

enum {
  PLAY_PAUSE = 2,
  SKIP_FORWARD = 3,
  SKIP_BACKWARD = 4,
  VOLUME_UP = 5,
  VOLUME_DOWN = 6
};

int playPauseButton, skipForwardButton, skipBackwardButton, \
  volumeUpButton, volumeDownButton = 0;

int connected = 0;

void writeRGB(int r, int g, int b) {
  analogWrite(LED_RED, r);
  analogWrite(LED_GREEN, g);
  analogWrite(LED_BLUE, b);
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
  
  // initialize led
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  // initialize bluetooth keyboard
  keyboard.begin();
  Serial.println("bluetooth started");

  delay(5000);
}

void loop() {
  if (keyboard.isConnected()) {
    if (!connected) {
      Serial.println("connected to device");
      writeRGB(0, 128, 0);
      connected = 1;
      delay(500);
    }

    playPauseButton = digitalRead(PLAY_PAUSE);
    skipForwardButton = digitalRead(SKIP_FORWARD);
    skipBackwardButton = digitalRead(SKIP_BACKWARD);
    volumeUpButton = digitalRead(VOLUME_UP);
    volumeDownButton = digitalRead(VOLUME_DOWN);

    Serial.println("P < > + -");
    Serial.printf("%d %d %d %d %d\n", playPauseButton, skipBackwardButton,
      skipForwardButton, volumeUpButton, volumeDownButton);
  }
  
  else {
    if (connected) {
      Serial.println("disconnected from device");
      connected = 0;
    }

    Serial.println("searching for bluetooth device...");

    // breathe blue for every second that there is no device
    analogWrite(LED_RED, 0);
    analogWrite(LED_GREEN, 0);
    for (int i = 0; i < 100; i++) {
      analogWrite(LED_BLUE, i+50);
      delay(5);
    }
    for (int i = 99; i >= 0; i--) {
      analogWrite(LED_BLUE, i+50);
      delay(5);
    }
  }

/* test code for reference or whatever
  if (keyboard.isConnected()) {
    Serial.println("connected; typing stuff");
    // Print "Hello, World!"
    keyboard.print("Hello, ");
    keyboard.println("World!");
    // Press and Tap "ESP32!"
    keyboard.press(KEY_LSHIFT);
    delay(25);
    keyboard.tap(KEY_E);
    keyboard.tap(KEY_S);
    keyboard.tap(KEY_P);
    keyboard.release(KEY_LSHIFT);
    keyboard.tap(KEY_3);
    keyboard.tap(KEY_2);
    // tap an exclamation point "!"
    keyboard.tap(KEY_1, KEY_MOD_LSHIFT);
    keyboard.tap(KEY_RETURN);
    
    keyboard.releaseAll();

    // wait for device to disconnect before running again
    while (keyboard.isConnected());
  }
*/
}
