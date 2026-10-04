#include <HijelHID_BLEKeyboard.h>

HijelHID_BLEKeyboard keyboard("bt-remote", "chuds", 100);

void setup() {
  // pinMode(LED)
  Serial.begin(115200);
  while (!Serial);
  
  keyboard.begin();
  Serial.println("bluetooth started");
}

void loop() {
  delay(5000);
  // Open a text editor on your host device
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
}
