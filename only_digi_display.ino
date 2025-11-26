#include <TM1637.h>

const int wrcPin = A1;
int CLK = 2;
int DIO = 4;

TM1637 tm(CLK,DIO);

void setup() {
  Serial.begin(9600);
 
  tm.init();

  tm.set(5);
}

void displayNumber(int num){   
    tm.display(3, num % 10);   
    tm.display(2, num / 10 % 10);   
    tm.display(1, num / 100 % 10);   
    tm.display(0, num / 1000 % 10);
}

void loop() {
  float v_coil = analogRead(wrcPin);
  //float v_coil = 600;
  float v_calc = ((v_coil * 5) / 1023) * 2; // convert to readable value and accounts for divider
  int v_disp = v_calc * 1000;
  displayNumber(v_disp);
  Serial.print(v_disp);

  delay(500);
}
