#include <i2c_t3.h>

#define ADDR         0x1D
#define CTRL_REG1    0x2A
#define OUT_X_MSB    0x01
#define OUT_X_LSB    0x02
#define OUT_Y_MSB    0x03
#define OUT_Y_LSB    0x04
#define OUT_Z_MSB    0x05
#define OUT_Z_LSB    0x06
#define ACTIVE_MASK  0x01

uint8_t read_reg(uint8_t addr){
  Wire.beginTransmission(ADDR);
  Wire.send(addr);
  int res = Wire.endTransmission(I2C_NOSTOP);
  //Serial.print("Register addr transmission status: ");
  //Serial.println(res);
  res = Wire.requestFrom(ADDR, 1);
  //Serial.print("Register data request status: ");
  //Serial.println(res);
  if(Wire.available())
    return Wire.readByte();
} 

void write_reg(uint8_t addr, uint8_t val){
  Wire.beginTransmission(ADDR);
  Wire.send(addr);
  int res = Wire.endTransmission(I2C_NOSTOP);
  //Serial.print("Register write status 1: ");
  //Serial.println(res);
  Wire.send(val);
  res = Wire.endTransmission(I2C_STOP);
  //Serial.print("Register write status 2: ");
  //Serial.println(res);
}

void standby_mode(){
  write_reg(CTRL_REG1, read_reg(CTRL_REG1) & ~ ACTIVE_MASK);
}

void active_mode(){
  write_reg(CTRL_REG1, read_reg(CTRL_REG1) | ACTIVE_MASK);
}

void setup(){
  Serial.begin(115200);
  Serial.println("Serial ready");
  Wire.begin(); //as master
  
  //config stuff
  active_mode();
  //write_reg(CTRL_REG1, 0x00);
}

void loop(){
  uint8_t test = 0;
  /*
  byte msb, lsb;
  msb = read_reg(OUT_X_MSB);
  lsb = read_reg(OUT_X_LSB);
  Serial.print("x: ");
  Serial.print(msb, HEX);
  Serial.println(lsb, HEX);
  
  msb = read_reg(OUT_Y_MSB);
  lsb = read_reg(OUT_Y_LSB);
  Serial.print("y: ");
  Serial.print(msb, HEX);
  Serial.println(lsb, HEX);
  
  msb = read_reg(OUT_Z_MSB);
  lsb = read_reg(OUT_Z_LSB);
  Serial.print("z: ");
  Serial.print(msb, HEX);
  Serial.println(lsb, HEX);
  */

  active_mode();
  test = read_reg(CTRL_REG1);
  Serial.println(test, BIN);
  standby_mode();
  test = read_reg(CTRL_REG1);
  Serial.println(test, BIN);
  
  delay(1000);
}
