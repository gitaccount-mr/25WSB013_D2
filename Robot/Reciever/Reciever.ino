#include <RH_ASK.h>
#include <SPI.h>
#include <TM1637Display.h>

#define CLK 4
#define DIO 5

TM1637Display display(CLK, DIO);
RH_ASK driver(2000, 2, 3); 

int lastValue = -1;
unsigned long lastGood = 0;

void setup() {
  display.setBrightness(7);
  display.showNumberDec(0, true);
  driver.init();
}

void loop() {
  uint8_t buf[20];
  uint8_t buflen = sizeof(buf);

  if (driver.recv(buf, &buflen)) {
    buf[buflen] = 0;

    int value;
    unsigned int cs;
    sscanf((char*)buf, "%d,%x", &value, &cs);

    byte check = (value / 1000 + value / 100 + value / 10 + value) & 0xFF;

    if (check == cs) {
      if (value != lastValue) {
        display.showNumberDec(value, true);
        lastValue = value;
      }
      lastGood = millis();
    }
  }

  if (millis() - lastGood > 2000) {
    display.clear();
  }
}
