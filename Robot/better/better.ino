#include <Servo.h>
#include <IRremote.h>

Servo leftServo;
Servo rightServo;

int receiver = 10;
uint32_t last_decodedRawData = 0;

IRrecv irrecv(receiver); // Create IR receiver object
decode_results results;

void translateIR() {
  // If it's a repeat signal, use last known code
  if (irrecv.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
    Serial.println("REPEAT signal");
    irrecv.decodedIRData.decodedRawData = last_decodedRawData;
  } else {
    Serial.print("IR code: 0x");
    Serial.println(irrecv.decodedIRData.decodedRawData, HEX);
    last_decodedRawData = irrecv.decodedIRData.decodedRawData;
  }

  // Handle IR commands
  switch (irrecv.decodedIRData.decodedRawData) {
    case 0xBA45FF00: // POWER button
      Serial.println("POWER (Forward)");
      leftServo.write(0);  // full forward
      rightServo.write(160);   // opposite direction forward
      delay(500);            // move a bit

      // stop both at the same time
      leftServo.write(90);
      rightServo.write(90);
         
      break;

    

    default:
      Serial.println("Other button pressed (No action)");
      break;
  }

  delay(250); // Delay to avoid repeated fast input
}


void setup() {
  leftServo.attach(11);
  rightServo.attach(12);
  Serial.println("System ready...");

  Serial.begin(9600);
  irrecv.enableIRIn();
}



void loop() {
  // both forward
            // stay still


  if (irrecv.decode()) {
    translateIR();
    irrecv.resume(); // Receive next IR signal
  }
  // repeat
}
