// SCCC
// Detection with digital display
// Relay activated to enter charging mode
// LED indicatiors display p.d. across SC

// SC p.d. reading
const int analogPin = A0;
float Time_s = 0;

// led indicators
const int greenLED = 10;
const int blueLED = 9;
const int redLED = 8;

unsigned long lastTransmitTime = 0;
const unsigned long TRANSMIT_INTERVAL = 200;
// store previous voltage
float previous_vcap = 0.0;

// digital display
const int wrcPin = A1;
float v_coil = 0;

// transciever
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 6);
const byte addresses[][6] = {"00001", "00002"};
int responseCode = 1234;

// relay switch
const int relayPin = 5;

// Parsed joystick data
int joyX = 93;    // 0–90 (93 = centred/still)
int joyY = 93;    // 90–180 (93 = centred/still)
int toggleState = 0; // 0 or 1

//=========================================================

void handleJoyX(int x) {
  // TODO: map xVal (0–90, or 93 for still) to your output
}

void handleJoyY(int y) {
  // TODO: map yVal (90–180, or 93 for still) to your output
}

void parseReceivedMessage(char* buf) {
  // Expected format: "45,135,1"
  char* token = strtok(buf, ",");
  if (token != NULL) { joyX = atoi(token); token = strtok(NULL, ","); }
  if (token != NULL) { joyY = atoi(token); token = strtok(NULL, ","); }
  if (token != NULL) { toggleState = atoi(token); }
}

//=========================================================

void setup() {
  Serial.begin(9600);
  Serial.println("System Initalising...");

  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  Serial.println("LED Setup Begin...");
  digitalWrite(blueLED, HIGH);  delay(400);
  digitalWrite(redLED, HIGH);   delay(400);
  digitalWrite(greenLED, HIGH); delay(400);
  digitalWrite(blueLED, LOW); digitalWrite(redLED, LOW); digitalWrite(greenLED, LOW); delay(200);
  digitalWrite(blueLED, HIGH); digitalWrite(redLED, HIGH); digitalWrite(greenLED, HIGH); delay(200);
  digitalWrite(blueLED, LOW); digitalWrite(redLED, LOW); digitalWrite(greenLED, LOW); delay(200);
  digitalWrite(blueLED, HIGH); digitalWrite(redLED, HIGH); digitalWrite(greenLED, HIGH); delay(200);
  digitalWrite(blueLED, LOW); digitalWrite(redLED, LOW); digitalWrite(greenLED, LOW); delay(200);
  digitalWrite(blueLED, HIGH); digitalWrite(redLED, HIGH); digitalWrite(greenLED, HIGH); delay(200);
  digitalWrite(blueLED, LOW); digitalWrite(redLED, LOW); digitalWrite(greenLED, LOW);
  Serial.println("LED Setup Complete");
  delay(50);

  Serial.println("Relay Setup Begin...");
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);  delay(400);
  digitalWrite(relayPin, HIGH); delay(400);
  digitalWrite(relayPin, LOW);
  Serial.println("Relay Setup Complete");

  Serial.println("Transceiver Setup Begin...");
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openReadingPipe(1, addresses[0]);
  radio.openWritingPipe(addresses[1]);
  radio.startListening();
  Serial.println("Transceiver Ready");

  Serial.println("System Initialised.");
  Serial.println("Enter 1 to run in default mode. Enter any other character to cancel.");

  while (Serial.available() == 0) {}

  int go = Serial.parseInt();
  if (go == 1) {
    Serial.println("Ready.");
    Serial.println("");
  } else {
    Serial.print("Cancelling... Reset Arduino to retry.");
  }

  Serial.print("\n");
  Serial.println("Time_s,Voltage_V,Rate_V/s,Coil_V,RelayState,JoyX,JoyY,Toggle");
}

//=======================================================

void loop() {

  unsigned long now = millis();

  // Listen for incoming joystick message
  if (radio.available()) {
    char buf[20] = {0};
    radio.read(buf, sizeof(buf));
    parseReceivedMessage(buf);

    // Call open functions for X and Y
    handleJoyX(joyX);
    handleJoyY(joyY);
  }

  if (now - lastTransmitTime >= TRANSMIT_INTERVAL) {
    lastTransmitTime = now;

    int raw = analogRead(analogPin);
    float v_adc = (raw * 5.0) / 1023.0;
    float v_cap = v_adc * 2.0;

    float rate = (v_cap - previous_vcap) / 0.5;

    if (rate > 0) {
      digitalWrite(blueLED, HIGH);
    } else {
      digitalWrite(blueLED, LOW);
    }

    if (v_cap > 2.0) {
      digitalWrite(redLED, HIGH);
    } else {
      digitalWrite(redLED, LOW);
      digitalWrite(greenLED, LOW);
    }

    if (v_cap > 4.8) {
      digitalWrite(greenLED, HIGH);
    } else {
      digitalWrite(greenLED, LOW);
    }

    v_coil = analogRead(wrcPin);
    float v_calc = ((v_coil * 5) / 1023);
    int v_disp = v_calc * 1000;

    // Relay controlled by toggle (last digit), same as before
    if (toggleState == 1) {
      digitalWrite(relayPin, HIGH);
    } else {
      digitalWrite(relayPin, LOW);
    }

    int dispRadio;
    if (toggleState == 1) {
      dispRadio = v_cap * 1000;
    } else {
      dispRadio = v_disp;
    }

    // CSV output
    Serial.print(Time_s);   Serial.print(",");
    Serial.print(v_cap, 4); Serial.print(",");
    Serial.print(rate, 4);  Serial.print(",");
    Serial.print(v_calc, 4);Serial.print(",");
    Serial.print(toggleState == 1 ? "ON" : "OFF"); Serial.print(",");
    Serial.print(joyX);     Serial.print(",");
    Serial.print(joyY);     Serial.print(",");
    Serial.println(toggleState);

    responseCode = dispRadio;
    radio.stopListening();
    radio.write(&responseCode, sizeof(responseCode));
    radio.startListening();

    previous_vcap = v_cap;
    Time_s += 0.2;
  }
}
