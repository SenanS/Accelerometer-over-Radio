#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>

//                Potentiometer global setup
#define pot A0
double scaleVal = 0.75;
int16_t filteredRes;

//                RF global setup
RF24 radio(9, 10);
const byte addresses[][6] = {"00001", "00002"};
//each accel value is max one byte (8 chars) long, three bytes is 24 chars long.
String accelData;

int counter;
bool noSignal = false;

void setup() {

  Serial.begin(115200);

  //                Potentiometer setup
  pinMode(pot, INPUT);
  filteredRes = map(analogRead(pot), 0, 1023, 0, 99);

  //                RF setup
  radio.begin();
  radio.openWritingPipe(addresses[1]);        // Write to address 2
  radio.openReadingPipe(1, addresses[0]);     // Read from address 1
  radio.setPALevel(RF24_PA_MIN);

}

void loop() {

  delay(5);
  radio.stopListening();
  
  //Smooth the resistance reading and scale it to a percent.
  filteredRes = scaleVal * filteredRes + (1 - scaleVal) * map(analogRead(pot), 0, 1023, 0, 100);
  //Send the potentiometer % over the writing pipe.
  
  radio.write(&filteredRes, sizeof(filteredRes));
//  Serial.print("Sent     : ");
//  Serial.println(filteredRes);


  if(filteredRes > 40){
    delay(5);
    //Change the program to receive mode & wait until a packet is received
    radio.startListening();
    counter = millis();
    while(!radio.available()){
      if(millis() > (counter + 50)){
        Serial.println("timeout");
        noSignal = true;
        break;
      }
    }
    if(!noSignal){
      char message[32];
      radio.read(&message, sizeof(message));
//      Serial.print("Received : ");
      Serial.println(message);
    }
  
  }
  noSignal = false;
}
