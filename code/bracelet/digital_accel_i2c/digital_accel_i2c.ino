#include <Wire.h>

void setup(){
  Wire.begin(); //as master
  Serial.begin(115200);
}

void loop(){
  Wire.requestFrom(0x1D, 1);
  while(Wire.available())
  {
    byte c = Wire.read();
    Serial.print("received: ");
    Serial.println(c, DEC);
  }
}
