// Supercapacitor P.D. Logging + LED display

const int analogPin = A0;
float Time_s = 0;

void setup() {
  Serial.begin(9600);
  
  // CSV header
  Serial.println("Time_s,Voltage_V");
}

void loop() {

  // Read A0 pin 
  int raw = analogRead(analogPin);

  // Convert ADC to accurate V
  float v_adc = (raw * 5.0) / 1023.0; // ADC value to actual voltage
  float v_cap = v_adc * 2.0;  // account for 10k/10k voltage divider and multiply by 2

  // CSV output
  Serial.print(Time_s);
  Serial.print(",");
  Serial.println(v_cap);

  Time_s += 0.5;   // increase by 0.5s

  delay(500);  // sample every 500 ms
}
