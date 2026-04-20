// Supercapacitor P.D. Logging + LED display

const int analogPin = A0;
float Time_s = 0;

//led indicators
const int greenLED = 10;
const int blueLED = 9;
const int redLED = 8;


// Store previous voltage
float previous_vcap = 0.0;

//digital display
#include <TM1637.h>
const int wrcPin = A1;
int CLK = 2;
int DIO = 4;
TM1637 tm(CLK,DIO);
float v_coil = 0;

void setup() {
  Serial.begin(9600);

  // CSV header
  Serial.print("\n");
  Serial.println("Time_s,Voltage_V,Rate_V_/_s");

  // set pin modes for LEDs
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  // initialise digi display and set brightness
  tm.init();
  tm.set(5);
}

// function for displaying coil value
void displayNumber(int num){   
    tm.display(3, num % 10);   
    tm.display(2, num / 10 % 10);   
    tm.display(1, num / 100 % 10);   
    tm.display(0, num / 1000 % 10);
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

  //digital display for coil voltage
  v_coil = analogRead(wrcPin);
  float v_calc = ((v_coil * 5) / 1023) * 2; // convert to readable value and accounts for divider
  int v_disp = v_calc * 1000; // allows the whole number to be diplayed on the screen at x10^3
  displayNumber(v_disp);

  // Update for next loop
  previous_vcap = v_cap;
  Time_s += 0.2;

  delay(200);
}
