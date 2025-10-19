#include <RH_ASK.h>
#include <SPI.h>

RH_ASK driver(2000, 11, 12, 0); // must match transmitter (rxPin, txPin swapped accordingly)

void setup() {
  Serial.begin(9600);
  driver.init();
}

void loop() {
  uint8_t buf[12];
  uint8_t buflen = sizeof(buf);
  if (driver.recv(buf, &buflen)) {
    buf[buflen] = '\0';          // add string terminator
    Serial.print("Message: ");
    Serial.println((char*)buf);
  }
}