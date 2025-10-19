#include <RH_ASK.h>
#include <SPI.h> // not used directly but required

RH_ASK driver(2000, 11, 12, 0); // (bitrate, rxPin, txPin, pttPin)

void setup() {
  driver.init();
}

void loop() {
  const char *msg = "Hello World!";
  driver.send((uint8_t *)msg, strlen(msg));
  driver.waitPacketSent();
  delay(1000);
}