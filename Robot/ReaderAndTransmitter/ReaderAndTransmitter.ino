#include <TM1637.h>      // (only needed if you still want local display)
#include <RH_ASK.h>
#include <SPI.h>

// ---------- RF ----------
RH_ASK driver(2000, 2, 3);  
// RX pin unused, TX pin = 3

// ---------- Supercap ----------
const int analogPin = A0;
float Time_s = 0;
float previous_vcap = 0.0;

// ---------- Coil voltage ----------
const int wrcPin = A1;

// ---------- LEDs ----------
const int greenLED = 10;
const int blueLED  = 9;
const int redLED   = 8;

// ---------- Timing ----------
unsigned long lastSampleTime = 0;
const unsigned long sampleInterval = 200; // ms

// ---------- RF state ----------
int lastSentValue = -1;

void setup() {
  Serial.begin(9600);
  driver.init();

  Serial.println("Time_s,Voltage_V,Rate_V_/_s");

  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(redLED, OUTPUT);
}

void loop() {
  unsigned long now = millis();

  // Run every 200 ms without blocking
  if (now - lastSampleTime >= sampleInterval) {
    lastSampleTime = now;
    Time_s += 0.2;

    // -------- Supercap reading --------
    int raw = analogRead(analogPin);
    float v_adc = (raw * 5.0) / 1023.0;
    float v_cap = v_adc * 2.0;

    float rate = (v_cap - previous_vcap) / 0.2;
    previous_vcap = v_cap;

    // CSV output
    Serial.print(Time_s);
    Serial.print(",");
    Serial.print(v_cap, 4);
    Serial.print(",");
    Serial.println(rate, 4);

    // -------- LEDs --------
    digitalWrite(blueLED, rate > 0);
    digitalWrite(redLED,  v_cap > 2.0);
    digitalWrite(greenLED, v_cap > 4.75);

    // -------- Coil voltage --------
    float v_coil = analogRead(wrcPin);
    float v_calc = (v_coil * 5.0) / 1023.0;
    int v_disp = v_calc * 1000;   // 0–5000

    // -------- RF send --------
    if (v_disp != lastSentValue) {
      sendValue(v_disp);
      lastSentValue = v_disp;
    }
  }
}

// ---------------- RF function ----------------
void sendValue(int v) {
  byte checksum = (v / 1000 + v / 100 + v / 10 + v) & 0xFF;

  char msg[10];
  sprintf(msg, "%04d,%02X", v, checksum);

  // send 3 times for reliability (still non-blocking-ish)
  for (int i = 0; i < 3; i++) {
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
  }
}
