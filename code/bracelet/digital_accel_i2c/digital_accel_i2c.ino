#include <Wire.h>

void setup(){
  Wire.begin(); //as master
  Serial.begin(115200);
  
  Wire.beginTransmission(0x1D);
  Wire.send(0x00);
  Wire.endTransmission();
  Wire.requestFrom(0x1D, 1);
  byte b = Wire.read();
  Serial.println(b, BIN);
}

void loop(){
  Wire.beginTransmission(0x1D);
  Wire.send(0x01);
  Wire.endTransmission();
  
  Wire.requestFrom(0x1D, 1);
  byte c = Wire.read();
  Serial.print("received: ");
  Serial.println(c, DEC);
  
  delay(1000);
}
