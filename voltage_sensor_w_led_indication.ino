// Supercapacitor P.D. Logging + LED display

const int analogPin = A0;
float Time_s = 0;

const int greenLED = 10;
const int blueLED = 9;
const int redLED = 8;


// Store previous voltage
float previous_vcap = 0.0;

void setup() {
  Serial.begin(9600);

  // CSV header
  Serial.print("\n");
  Serial.println("Time_s,Voltage_V,Rate_V_/_s");

  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(redLED, OUTPUT);

}

void loop() {
 
  // Read A0 pin
  int raw = analogRead(analogPin);
 
  // Convert ADC to accurate voltage
  float v_adc = (raw * 5.0) / 1023.0;  
  float v_cap = v_adc * 2.0; // account for divider

  // Calculate rate of change (V/s)
  float rate = (v_cap - previous_vcap) / 0.5;

  // Output CSV
  Serial.print(Time_s);
  Serial.print(",");
  Serial.print(v_cap, 4);
  Serial.print(",");
  Serial.println(rate, 4);

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

  // Update for next loop
  previous_vcap = v_cap;
  Time_s += 0.5;

  delay(500);
}
