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

byte read_reg(byte addr){
  Wire.beginTransmission(ADDR);
  Wire.send(addr);
  int res = Wire.endTransmission(I2C_NOSTOP);
  //Serial.print("Register addr transmission status: ");
  //Serial.println(res);
  res = Wire.requestFrom(ADDR, 1);
  //Serial.print("Register data request status: ");
  //Serial.println(res);
  if(Wire.available())
    return Wire.read();
} 

void write_reg(byte addr, byte val){
  Wire.beginTransmission(ADDR);
  Wire.send(addr);
  Wire.send(val);
  int res = Wire.endTransmission();
  Serial.print("Register write status: ");
  Serial.println(res);
}

void active_mode(){
  write_reg(CTRL_REG1, read_reg(CTRL_REG1) & ~ ACTIVE_MASK);
}

void standby_mode(){
  write_reg(CTRL_REG1, read_reg(CTRL_REG1) | ACTIVE_MASK);
}

void setup(){
  Serial.begin(115200);
  Serial.println("Serial ready");
  Wire.begin(); //as master
  
  //config stuff
  active_mode();
}

void loop(){
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
  //active_mode();
  byte test = read_reg(0x0D);
  Serial.println(test, BIN);
  
  delay(1000);
}
