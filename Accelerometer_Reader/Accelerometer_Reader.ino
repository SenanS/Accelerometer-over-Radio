#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
#include <Wire.h>

//                RF tranceiver global setup
RF24 radio(9, 10);
const byte addresses[][6] = {"00001", "00002"};

//                Accel sensor global setup
//I2C mem address
const int MPU_ADDR = 0x68;
//3 axis vars        
int16_t accelerometer_x, accelerometer_y, accelerometer_z, potPercent = 0; 

//                Accel filter variables setup
double scaleVal = 0.85;
int16_t filtered_x = 0, filtered_y = 5000, filtered_z = 10000, shiftVal = 4465;

void setup() {
  
  Serial.begin(115200);
  //                RF setup
  radio.begin();
  radio.openWritingPipe(addresses[0]);        // Write to address 1
  radio.openReadingPipe(1, addresses[1]);     // Read from address 2
  radio.setPALevel(RF24_PA_MIN);

  //                Accel setup
  //Wake up accelerometer
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); 
  Wire.write(0); 
  Wire.endTransmission(true);
  
   delay(5);
   radio.startListening();
}

void loop() {
  delay(5);
  radio.startListening();
  
  if ( radio.available()) {
    
    while (radio.available()) {
      potPercent = 0;
      radio.read(&potPercent, sizeof(potPercent));
      Serial.print("Received : ");
      Serial.println(potPercent);
    }
    if(potPercent > 40){
      delay(5);
      radio.stopListening();

      //Begin transmission to I2C address
      Wire.beginTransmission(MPU_ADDR);
      //At register 0x3b
      Wire.write(0x3B); 
      //Keep connection active
      Wire.endTransmission(false); 
      //Get 14 registers
      Wire.requestFrom(MPU_ADDR, 7*2, true); 
    
      //Read accel data using logical shifts to navigate
      //Read Accel x byte 0x3B & 0x3C
      accelerometer_x = Wire.read()<<8 | Wire.read(); 
      //Read Accel y byte 0x3D & 0x3E
      accelerometer_y = Wire.read()<<8 | Wire.read(); 
      //Read Accel z byte 0x3F & 0x40
      accelerometer_z = Wire.read()<<8 | Wire.read(); 
      
    
      //LP Filter of accelerometer vals & shifting the average value to create easily legible plot
      filtered_x = scaleVal * filtered_x + (1 - scaleVal) * accelerometer_x;
      filtered_y = scaleVal * filtered_y + (1 - scaleVal) * (accelerometer_y + shiftVal);
      filtered_z = scaleVal * filtered_z + (1 - scaleVal) * (accelerometer_z - shiftVal);
      
      //Format Data
      String accelValues = String(filtered_x);
      accelValues.concat(" ");
      accelValues.concat(filtered_y);
      accelValues.concat(" ");
      accelValues.concat(filtered_z);
//      Serial.print("Sent     : ");
//      Serial.println(accelValues);
      char message[32];
      accelValues.toCharArray(message, 32);      
      radio.write(&message, sizeof(message));
    }
  } 
}
