#include <RH_ASK.h>
#include <SPI.h>

RH_ASK driver(2000, 2, 3); 
// TX pin = 3

int lastValue = -1;
int fakeValue = 0;

void setup() {
  driver.init();
  randomSeed(analogRead(A0));
}

void loop() {
  // Dummy changing number (0000 → 9999)
  fakeValue++;
  if (fakeValue > 9999) fakeValue = 0;

  if (fakeValue != lastValue) {
    sendValue(fakeValue);
    lastValue = fakeValue;
  }

  delay(200);   // change speed
}

void sendValue(int v) {
  byte checksum = (v / 1000 + v / 100 + v / 10 + v) & 0xFF;

  char msg[10];
  sprintf(msg, "%04d,%02X", v, checksum);

  for (int i = 0; i < 3; i++) {
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
    delay(random(10, 40));  
  }
}
