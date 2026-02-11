#include <RH_ASK.h>
#include <SPI.h> // Needed even if not used directly

// Create RF driver on pin 11
RH_ASK rf_driver(2000, 8); 
// Params: (bitrate, rxPin, txPin, pttPin)
// txPin and pttPin are unused here, just placeholders.

const String key = "ABC123";  // Shared secret key
char message[32]; // Buffer for incoming data

void setup() {
  Serial.begin(9600);
  if (!rf_driver.init()) {
    Serial.println("RF init failed!");
  } else {
    Serial.println("RF Receiver ready (Pin 8)");
  }
}

void loop() {
  uint8_t buf[32];
  uint8_t buflen = sizeof(buf);

  if (rf_driver.recv(buf, &buflen)) {
    buf[buflen] = '\0'; // Null-terminate the received data
    String received = String((char*)buf);

    // Verify message starts with correct key
    if (received.startsWith(key)) {
      String payload = received.substring(key.length());
      Serial.print("Valid signal received: ");
      Serial.println(payload);

      // TODO: Add actions for payload like "FWD", "BACK", etc.
    } else {
      Serial.println("Invalid key, signal ignored");
    }
  }
}
