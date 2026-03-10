#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

// ── Pin definitions ───────────────────────────────────────
const int analogPin = A0;   // supercap voltage sense
const int wrcPin    = A1;   // coil voltage sense
const int greenLED  = 2;
const int blueLED   = 9;
const int redLED    = 8;
const int relayPin  = 5;
#define SERVO1_PIN  10
#define SERVO2_PIN  3

// ── Tuning ────────────────────────────────────────────────
#define CHANGE_MIN      1     // suppress ±1 servo flicker
#define TRANSMIT_MS   200     // sensor report interval (ms)

// ── Objects ───────────────────────────────────────────────
RF24 radio(7, 6);
const byte addresses[][6] = {"00001", "00002"};
Servo leftServo, rightServo;

// ── State ─────────────────────────────────────────────────
int servo1 = 93, servo2 = 93, toggleState = 0;
int lastServo1 = 93, lastServo2 = 93;
int responseCode = 0;
float previous_vcap = 0.0;
int cycles = 0;
unsigned long lastTransmitTime = 0;

// ── Servo helpers ─────────────────────────────────────────
void applyServos() {
  leftServo.write(servo1);
  rightServo.write(servo2);
}

void parseMessage(char* buf) {
  char* token = strtok(buf, ",");
  if (token != NULL) { servo1     = atoi(token); token = strtok(NULL, ","); }
  if (token != NULL) { servo2     = atoi(token); token = strtok(NULL, ","); }
  if (token != NULL) { toggleState = atoi(token); }
}

// ── LED boot sequence ─────────────────────────────────────
void ledBoot() {
  int pins[] = {blueLED, redLED, greenLED};
  for (int i = 0; i < 3; i++) { digitalWrite(pins[i], HIGH); delay(400); }
  for (int f = 0; f < 3; f++) {
    for (int i = 0; i < 3; i++) digitalWrite(pins[i], f % 2 == 0 ? LOW : HIGH);
    delay(200);
  }
  for (int i = 0; i < 3; i++) digitalWrite(pins[i], LOW);
}

// ─────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);
  Serial.println("System Initialising...");

  leftServo.attach(SERVO1_PIN);
  rightServo.attach(SERVO2_PIN);
  leftServo.write(93);   // match controller centre value
  rightServo.write(93);

  pinMode(greenLED, OUTPUT);
  pinMode(blueLED,  OUTPUT);
  pinMode(redLED,   OUTPUT);
  ledBoot();

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  delay(400); digitalWrite(relayPin, HIGH);
  delay(400); digitalWrite(relayPin, LOW);

  radio.begin();
  radio.setChannel(100);        // add this
  radio.setPayloadSize(20);
  radio.setPALevel(RF24_PA_LOW);
  radio.openReadingPipe(1, addresses[0]);
  radio.openWritingPipe(addresses[1]);
  radio.startListening();

  Serial.println("System Initialised.");
  Serial.println("Enter 1 to run, any other key to cancel.");
  while (Serial.available() == 0) {}
  if (Serial.parseInt() != 1) {
    Serial.println("Cancelled. Reset to retry.");
    while (true) {}   // halt
  }

  Serial.println("Ready.\n");
  Serial.println("Cycles,Voltage_V,Rate_V/s,Coil_V,Relay,Servo1,Servo2,Toggle");
}

// ─────────────────────────────────────────────────────────
void loop() {
  unsigned long now = millis();

  // ── Receive & apply servos ─────────────────────────────
  if (radio.available()) {
    char buf[20] = {0};
    radio.read(buf, sizeof(buf));
    parseMessage(buf);

    // Suppress ±1 noise flicker
    if (abs(servo1 - lastServo1) <= CHANGE_MIN) servo1 = lastServo1;
    if (abs(servo2 - lastServo2) <= CHANGE_MIN) servo2 = lastServo2;

    // Only write to servo if value actually changed
    if (servo1 != lastServo1 || servo2 != lastServo2) {
      applyServos();
      lastServo1 = servo1;
      lastServo2 = servo2;
    }
  }

  // ── Sensor report + feedback transmit ─────────────────
  if (now - lastTransmitTime >= TRANSMIT_MS) {
    lastTransmitTime = now;

    // Supercap voltage
    int raw    = analogRead(analogPin);
    float v_cap = (raw * 5.0 / 1023.0) * 2.0;
    float rate  = (v_cap - previous_vcap) / (TRANSMIT_MS / 1000.0);
    previous_vcap = v_cap;

    // Coil voltage
    float v_coil = (analogRead(wrcPin) * 5.0) / 1023.0;

    // LEDs
    digitalWrite(blueLED,  rate > 0   ? HIGH : LOW);
    digitalWrite(redLED,   v_cap > 2.0 ? HIGH : LOW);
    digitalWrite(greenLED, v_cap > 4.8 ? HIGH : LOW);

    // Relay
    digitalWrite(relayPin, toggleState == 1 ? HIGH : LOW);

    // Display value: cap voltage in charge mode, coil voltage otherwise
    responseCode = (toggleState == 1)
                 ? (int)(v_cap  * 1000)
                 : (int)(v_coil * 1000);

    Serial.print(cycles);        Serial.print(",");
    Serial.print(v_cap,  4);     Serial.print(",");
    Serial.print(rate,   4);     Serial.print(",");
    Serial.print(v_coil, 4);     Serial.print(",");
    Serial.print(toggleState == 1 ? "ON" : "OFF"); Serial.print(",");
    Serial.print(servo1);        Serial.print(",");
    Serial.print(servo2);        Serial.print(",");
    Serial.println(toggleState);

    radio.stopListening();
    radio.write(&responseCode, sizeof(responseCode));
    radio.startListening();

    cycles++;
  }
}
