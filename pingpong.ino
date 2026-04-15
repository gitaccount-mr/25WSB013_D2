#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(addresses[1]);    // Sends on pipe 2
  radio.openReadingPipe(1, addresses[0]); // Receives on pipe 1
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    int receivedNum;
    radio.read(&receivedNum, sizeof(receivedNum));
    Serial.print("Received: ");
    Serial.println(receivedNum);

    // Prepare response (received number + 1)
    int replyNum = receivedNum + 1;
    
    delay(10); // Small delay to let Node A switch to listening mode
    radio.stopListening();
    radio.write(&replyNum, sizeof(replyNum));
    Serial.print("Sent reply: ");
    Serial.println(replyNum);
    
    radio.startListening();
  }
}
