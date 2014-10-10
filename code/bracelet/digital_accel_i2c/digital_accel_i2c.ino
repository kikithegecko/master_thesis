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

struct AccelData{
  int x;
  int y;
  int z;
};

uint8_t read_reg(uint8_t addr){
  Wire.beginTransmission(ADDR);
  Wire.send(addr);
  int res = Wire.endTransmission(I2C_NOSTOP);
  res = Wire.requestFrom(ADDR, 1);
  if(Wire.available())
    return Wire.readByte();
}

void write_reg(uint8_t addr, uint8_t val){
  Wire.beginTransmission(ADDR);
  Wire.send(addr);
  int res = Wire.endTransmission(I2C_NOSTOP);
  Wire.send(val);
  res = Wire.endTransmission(I2C_STOP);
}

void standby_mode(){
  write_reg(CTRL_REG1, read_reg(CTRL_REG1) & ~ ACTIVE_MASK);
}

void active_mode(){
  write_reg(CTRL_REG1, read_reg(CTRL_REG1) | ACTIVE_MASK);
}

int bytes_to_int(uint8_t high, uint8_t low){
  uint16_t temp = (high << 8) + low;
  int factor = 1;
  
  if (high > 0x7F)
  {
    // negative number
    temp = (~ temp) + 1;
    factor = -1;
  }
  
  return (temp >> 4) * factor;
}
  
struct AccelData get_acceleration_data(){
  struct AccelData data;
  
  uint8_t x_hi = read_reg(OUT_X_MSB);
  uint8_t x_lo = read_reg(OUT_X_LSB);
  
  data.x = bytes_to_int(x_hi, x_lo);
  
  uint8_t y_hi = read_reg(OUT_Y_MSB);
  uint8_t y_lo = read_reg(OUT_Y_LSB);
  
  data.y = bytes_to_int(y_hi, y_lo);  
  
  uint8_t z_hi = read_reg(OUT_Z_MSB);
  uint8_t z_lo = read_reg(OUT_Z_LSB);
  
  data.z = bytes_to_int(z_hi, z_lo);
  
  return data;
}

void setup(){
  Serial.begin(115200);
  Wire.begin(); //as master
  
  //config stuff
  write_reg(CTRL_REG1, 0x00); //to clear previous config
  active_mode();
}

void loop(){
  uint8_t test = 0;
  
  /*
  uint8_t msb, lsb;
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
  
  struct AccelData data = get_acceleration_data();
  Serial.print("x: ");
  Serial.println(data.x);
  Serial.print("y: ");
  Serial.println(data.y);
  Serial.print("z: ");
  Serial.println(data.z);
  
  
  /*
  active_mode();
  test = read_reg(CTRL_REG1);
  Serial.println(test, BIN);
  standby_mode();
  test = read_reg(CTRL_REG1);
  Serial.println(test, BIN);
  */
  
  delay(1000);
}
