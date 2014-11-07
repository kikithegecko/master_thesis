#include <i2c_t3.h>
#include "threedollar.h"

/* Pin Configuration */
#define LED_R 4
#define LED_G 5
#define LED_B 3

/* I2C Configuration */
#define ADDR         0x1D

/* Register Addresses */
#define CTRL_REG1    0x2A
#define PULSE_CFG    0x21
#define PULSE_SRC    0x22
#define PULSE_THSZ   0x25
#define PULSE_TMLT   0x26
#define PULSE_LTCY   0x27
#define PULSE_WIND   0x28
#define XYZ_DATA_CFG 0x0E
#define STATUS_REG   0x00
#define OUT_X_MSB    0x01
#define OUT_X_LSB    0x02
#define OUT_Y_MSB    0x03
#define OUT_Y_LSB    0x04
#define OUT_Z_MSB    0x05
#define OUT_Z_LSB    0x06

/* Bitmasks for Configuration and Status Checks*/
#define ACTIVE_MASK  0x01
#define DATA_RDY     0x04
#define TAP_Z_SINGLE_EN 0xD0
#define TAP_Z_DOUBLE_EN 0xE0
#define TAP_Z_S_MASK 0xC0
#define TAP_Z_D_MASK 0xC8
#define HPF_EN_MASK  0x10

/* Gesture Recognizer Configuration */
#define SAMPLE_SIZE 150 //number of samples per Gesture
#define DELAY_SIZE 10 //delay betweeen two samples

struct AccelData{
  int x;
  int y;
  int z;
};

AccelData gesture[SAMPLE_SIZE];

/* Wrapper Function for Reading the Contents of a Register */
uint8_t read_reg(uint8_t addr){
  Wire.beginTransmission(ADDR);
  Wire.send(addr);
  int res = Wire.endTransmission(I2C_NOSTOP);
  res = Wire.requestFrom(ADDR, 1);
  if(Wire.available())
    return Wire.readByte();
}

/* Wrapper Function for Storing a given Byte in a Register */
void write_reg(uint8_t addr, uint8_t val){
  Wire.beginTransmission(ADDR);
  Wire.send(addr);
  int res = Wire.endTransmission(I2C_NOSTOP);
  Wire.send(val);
  res = Wire.endTransmission(I2C_STOP);
}

/* Sets the Accelerometer to Standby Mode.
 * Many Configurations can only be done while in Standby Mode 
 */
void standby_mode(){
  write_reg(CTRL_REG1, read_reg(CTRL_REG1) & ~ ACTIVE_MASK);
}

/* Sets the Accelerometer to Active Mode and Enables Data Recording */
void active_mode(){
  write_reg(CTRL_REG1, read_reg(CTRL_REG1) | ACTIVE_MASK);
}

/* Converts a 12-bit 2's complement Number stored in 
 * two 8-bit Registers to a signed integer number.
*/
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

/* Returns a Struct containing X, Y, and Z Acceleration Data
 * for the Point in Time when the Function is called.
 */
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

/* When a Pulse was detected, this checks if it was a
 * single or double tap and does stuff in each case.
 */
void process_pulse(uint8_t register_data){
  if(register_data & TAP_Z_S_MASK){ 
    if(register_data & 0x08){
      //signal double tap)
      analogWrite(LED_B, 255);
      delay(100);
      analogWrite(LED_B, 0);
      delay(50);
      analogWrite(LED_B, 255);
      delay(100);
      analogWrite(LED_B, 0);
      Serial.println("double tap");
      
      //record accelerometer data for the next SAMPLE_SIZE datasets
      analogWrite(LED_R, 255);
      Serial.println("start recording");
      for(int i = 0; i < SAMPLE_SIZE; i++){
        gesture[i] = get_acceleration_data();
        Serial.print(gesture[i].x / 1024.0);
        Serial.print(" ");
        Serial.print(gesture[i].y / 1024.0);
        Serial.print(" ");
        Serial.println(gesture[i].z / 1024.0);
        delay(DELAY_SIZE);
      }
      analogWrite(LED_R, 0);
      Serial.println("end recording");
    }
    else{
      //signal single tap
      analogWrite(LED_G, 255);
      delay(200);
      analogWrite(LED_G, 0);
      Serial.println("single tap");
    }
  }  
}

/* The usual setup stuff, setting pins, configuring the accelerometer
 * and starting serial communication.
 */
void setup(){
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  
  Serial.begin(115200);
  Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_INT, I2C_RATE_800);
  
  //config stuff
  write_reg(CTRL_REG1, 0x00); //to clear previous config
  write_reg(XYZ_DATA_CFG, HPF_EN_MASK); //output high-pass filtered data
  
  //2g mode is default (1024 counts per g)
  
  //enable tap-detection on z-axis
  write_reg(PULSE_CFG, (TAP_Z_SINGLE_EN | TAP_Z_DOUBLE_EN));
  //configure tap detection threshold, with steps of 0.063g/LSB @+/-8g range
  write_reg(PULSE_THSZ, 100); //default: 100
  //maximum time interval between the start impulse and the end impulse of a tap
  //with steps of 0.625ms/LSB @800MHz ODR with normal power mode and no LP filtering
  write_reg(PULSE_TMLT, 10);
  //define time interval after pulse detection in which other pulses are ignored
  //with steps of 2.5ms/LSB @800MHz ODR with normal power mode
  write_reg(PULSE_LTCY, 10); //25ms
  //configure maximum time between two taps of a double tap
  //with steps of 2.5ms/LSB @800MHz ODR with normal power mode
  write_reg(PULSE_WIND, 200); //500ms
  
  active_mode();
}


void loop(){
  
  /*
  if(read_reg(STATUS_REG) & DATA_RDY){
    struct AccelData data = get_acceleration_data();
    Serial.print("x: ");
    Serial.println(data.x / 1024.0);
    Serial.print("y: ");
    Serial.println(data.y / 1024.0);
    Serial.print("z: ");
    Serial.println(data.z / 1024.0);
  }
  */
  
  //need to save this because data is reset after reading
  uint8_t pulse_data = read_reg(PULSE_SRC);
  
  if(pulse_data & 0x80){
    process_pulse(pulse_data);
  }
  
  //delay(1000);
}
