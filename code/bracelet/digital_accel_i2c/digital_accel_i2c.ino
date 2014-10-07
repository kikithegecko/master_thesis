#include <Wire.h>

#define ADDR         0x1D
#define CTRL_REG1    0x2A
#define ACTIVE_MASK  0x01

byte read_reg(byte addr){
  Wire.beginTransmission(ADDR);
  Wire.send(addr);
  Wire.endTransmission();
  Wire.requestFrom(ADDR, 1);
  return Wire.read();
} 

void write_reg(byte addr, byte val){
  Wire.beginTransmission(ADDR);
  Wire.send(addr);
  Wire.send(val);
  Wire.endTransmission();
}

void active_mode(){
  byte current = read_reg(CTRL_REG1);

void setup(){
  Serial.begin(115200);
  Serial.println("Serial ready");
  Wire.begin(); //as master
  
  Wire.beginTransmission(ADDR);
  Wire.send(0x2A);
  Wire.send(0x01); //set active
  Wire.endTransmission();
  //Wire.requestFrom(0x1D, 1);
  //byte b = Wire.read();
  //Serial.println(b, BIN);
  //Serial.println("---");

}

void loop(){
  Wire.beginTransmission(ADDR);
  Wire.send(0x01);
  Wire.endTransmission();
  
  Wire.requestFrom(0x1D, 1);
  byte c = Wire.read();
  //Serial.print("received: ");
  Serial.println(c, BIN);
  
  delay(1000);
}
