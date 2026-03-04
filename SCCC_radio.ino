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
char receivedCommand;
int responseCode = 1234;

// relay switch
const int relayPin = 5;

//=========================================================

void setup() {
  Serial.begin(9600);

  Serial.println("System Initalising...");

  // set pin modes for LEDs
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  Serial.println("LED Setup Begin...");
  digitalWrite(blueLED, HIGH);
  delay(400);
  digitalWrite(redLED, HIGH);
  delay(400);
  digitalWrite(greenLED, HIGH);
  delay(400);
  digitalWrite(blueLED, LOW);
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);
  delay(200);
  digitalWrite(blueLED, HIGH);
  digitalWrite(redLED, HIGH);
  digitalWrite(greenLED, HIGH);
  delay(200);
  digitalWrite(blueLED, LOW);
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);
  delay(200);
  digitalWrite(blueLED, HIGH);
  digitalWrite(redLED, HIGH);
  digitalWrite(greenLED, HIGH);
  delay(200);
  digitalWrite(blueLED, LOW);
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);
    delay(200);
  digitalWrite(blueLED, HIGH);
  digitalWrite(redLED, HIGH);
  digitalWrite(greenLED, HIGH);
  delay(200);
  digitalWrite(blueLED, LOW);
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);
  Serial.println("LED Setup Complete");
  delay(50);

  // set pinmode for relay
  Serial.println("Relay Setup Begin...");
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  delay(400);
  digitalWrite(relayPin, HIGH);
  delay(400);
  digitalWrite(relayPin, LOW);
  Serial.println("Relay Setup Complete");

  // tranceiver setup
  Serial.println("Transceiver Setup Begin...");
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openReadingPipe(1, addresses[0]);
  radio.openWritingPipe(addresses[1]);
  radio.startListening();
  Serial.println("Transceiver Ready");

  // ready to begin
  Serial.println("System Initialised.");
  Serial.println("Enter 1 to run in default mode. Enter any other character to cancel.");
  
  while (Serial.available() == 0) {
  }

  int go = Serial.parseInt();

  if (go == 1) {
    Serial.println("Ready.");
    Serial.println("");
  } else {
    Serial.print("Cancelling... Reset Arduino to retry.");
  }

  // CSV header
  Serial.print("\n");
  Serial.println("Time_s,Voltage_V,Rate_V/s,Coil_V,RelayState");
}

//=======================================================

void loop() {
 
  // read A0 pin
  int raw = analogRead(analogPin);
 
  // Convert ADC to accurate voltage
  float v_adc = (raw * 5.0) / 1023.0;  
  float v_cap = v_adc * 2.0; // account for divider

  // calculate rate of change (V/s)
  float rate = (v_cap - previous_vcap) / 0.5;

  // conditions for LEDs
  if (rate > 0) {
    digitalWrite(blueLED,HIGH);
  }
  else {
    digitalWrite(blueLED,LOW);
  }

  if (v_cap > 2.0) {
    digitalWrite(redLED,HIGH);
  }
  else {
    digitalWrite(redLED,LOW);
    digitalWrite(greenLED,LOW);
  }
  
  if (v_cap > 4.8) {
    digitalWrite(greenLED,HIGH);
  }
  else {
    digitalWrite(greenLED,LOW);
  }

  // digital display for coil voltage
  v_coil = analogRead(wrcPin);
  float v_calc = ((v_coil * 5) / 1023); // convert to readable value and accounts for divider
  int v_disp = v_calc * 1000; // allows the whole number to be diplayed on the screen at x10^3

  // output CSV
  Serial.print(Time_s);
  Serial.print(",");
  Serial.print(v_cap, 4);
  Serial.print(",");
  Serial.print(rate, 4);
  Serial.print(",");
  Serial.print(v_calc, 4);
  Serial.print(",");

  // relay control
  if (radio.available()) {

  radio.read(&receivedCommand, sizeof(receivedCommand));
  Serial.print("Received: ");
  Serial.print(receivedCommand);
  Serial.print(", ");
  }

  int dispRadio;

  if (receivedCommand == '1') {
    digitalWrite(relayPin, HIGH);
    Serial.println("ON");
    dispRadio = v_cap * 1000;
  } else if (receivedCommand == '0') {
    digitalWrite(relayPin, LOW);
    Serial.println("OFF");
    dispRadio = v_disp;
  } else {
    Serial.println("NO INPUT");
  }

  // Send response back
  responseCode = dispRadio;
  radio.stopListening();
  radio.write(&responseCode, sizeof(responseCode));
  radio.startListening();

  // update for next loop
  previous_vcap = v_cap;
  Time_s += 0.2;

  // delay for stability
  delay(200);
}
