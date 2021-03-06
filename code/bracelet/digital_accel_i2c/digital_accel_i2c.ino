#include <i2c_t3.h>
#include <SoftwareSerial.h>
//#include "threedollar.h"
#include "TouchSlider.h"

/* Pin Configuration */
#define LED_R 4
#define LED_G 3
#define LED_B 5
#define BT_RX 7
#define BT_TX 8
#define BT_WAKE_HW 6
#define TOUCH_1 16
#define TOUCH_2 1
#define TOUCH_3 23
#define TOUCH_4 15
#define TOUCH_5 17
#define TOUCH_6 22
#define TOUCH_7 0

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
#define TAP_Z_D_MASK 0xC8ttslider
#define HPF_EN_MASK  0x10

/* Gesture Recognizer Configuration */
#define SAMPLE_SIZE 150 //number of samples per Gesture
#define DELAY_SIZE 10 //delay betweeen two samples

/* Capacitive Touch Configuration */
#define SEGMENT_COVER_THRESH 10 //number of cycles before a touch segment counts as touched !!!check this if delay functions are removed!

struct AccelData{
  int x;
  int y;
  int z;
};

AccelData gesture[SAMPLE_SIZE];
SoftwareSerial bluetooth(BT_RX, BT_TX);
const int TOUCH_PINS[] = {TOUCH_1, TOUCH_2, TOUCH_3, TOUCH_4, TOUCH_5, TOUCH_6, TOUCH_7};
TouchSlider tslider(7, TOUCH_PINS);
int touch_state[7]; //contains information on how long a touch element has been pressed
int red;
int green;
int blue;
int last_red;
int last_green;
int last_blue;

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
int process_pulse(uint8_t register_data){
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
      return 2; //double tap
    }
    else{
      //signal single tap
      analogWrite(LED_G, 255);
      delay(200);
      analogWrite(LED_G, 0);
      Serial.println("single tap");
      return 1; //single tap
    }
  }
  else{
    return 0; //no tap
  }  
}

/* After a cover was detected, this function is called.
 * It differentiates between the on/off hold and the dimming
 * gesture.
 */
void process_rotation(){
  float rotation[4]; //magic number!
  int i = 0;
  int rotation_count = 0;
  int mean_hold = 0;
  int debounce_onoff = 0;
  
  while(isCovered()){  
    AccelData data = get_acceleration_data();
    float x = data.x / 1024.0;
    float y = data.y / 1024.0;
    float z = (data.z / 1024.0) - 1; //subtract gravity
    rotation[i] = (atan(y / (sqrt(x*x + z*z))) * 4086) / 71; //x axis //<- use this!
    //look for twist
    float delta = rotation[((i-1) % 4)] - rotation[i];
    if((delta > 0.5) && (rotation[i] < -30.0)){ //more light
      Serial.print("<-- | ");
      Serial.println(rotation[i]);
      rotation_count++;
      //TODO dim
      red = red - red / 10;
      if(red < 0){
        red = 0;
      }
      green = green - green / 10;
      if(green < 0){
        green = 0;
      }
      blue = blue - blue / 10;
      if(blue < 0){
        blue = 0;
      }
    }
    else if((delta < -0.5) && (rotation[i] > -10.0)){ //less light
      Serial.print("--> | ");
      Serial.println(rotation[i]);
      rotation_count++;
      //TODO dim
      red = red + red / 10;
      if(red > 255){
        red = 255;
      }
      green = green + green / 10;
      if(green > 255){
        green = 255;
      }
      blue = blue + blue / 10;
      if(blue > 255){
        blue = 255;
      }
    }
    i = (i + 1) % 4;
    for(int k = 0; k < 7; k++){
      mean_hold += touch_state[i];
    }
    mean_hold /= 7;
    if((debounce_onoff == 0) && (mean_hold > 100) && (rotation_count < 10)){ //magic numbers!
      Serial.println("ON/OFF");
      debounce_onoff = 1;
      //on/off     
      if((red != 0) && (blue != 0) && (green != 0)){
        last_red = red;
        last_green = green;
        last_blue = blue;
        red = 0;
        blue = 0;
        green = 0;
      }
      else {
        red = last_red;
        green = last_green;
        blue = last_blue;
      }
    }
    send_to_lamp();
    delay(10);
  }
}

/* Checks if the whole touch surface is (still) covered */
int isCovered(){
  tslider.read();
  for(int i = 0; i < 7; i++){
    int val = tslider.getValue(i);
    if(val > 200){ //contact
      touch_state[i]++;
    }
    else if(touch_state[i] > 0){
      touch_state[i] = 0; //release contact
    }
  }
  int segments_covered = 0;
  for(int i = 0; i < 7; i++){
    if(touch_state[i] > SEGMENT_COVER_THRESH){
      segments_covered++;
    }
  }
  if(segments_covered >= 6){
    return 1;
  }
  else{
    return 0;
  }
}

/* changes the color mood to a given direction,
 * param indicates the direction
 * if param < 0, mood becomes more relaxing/blue
 * if param > 0, mood becomes more activating (orange/red)
 */
void process_mood_change(int param){
  //TODO change mood
  if(param < 0){ //increase blue, decrease red
    blue++;
    if(blue > 255){
      blue = 255;
    }
    red--;
    if(red < 0){
      red = 0;
    }
  }
  else if(param > 0){ //increase red, decrease blue
    red++;
    if(red > 255){
      red = 255;
    }
    blue--;
    if(blue < 0){
      blue = 0;
    }
  }
  
  send_to_lamp();
}

/* This function handles the precise color change
 * by tapping and sliding. Procedure is as follows:
 * 1. touch and hold the middle segment to activate
 *    this mode (implemented outside this function)
 * 2. slide to change hue
 * 3. tap on the electronics to change to the next
 *    parameter
 * 4. slide to set the saturation
 * 5. tap to confirm
 * 6. slide to set the lightness
 * 7. tap to confirm and exit this mode.
 * All changes are applied in real-time.
 */
void process_hue_change(){
  int pos = -1;
  int tap_detect = 0;
  int8_t pulse_data = 0;
  int hue = 0; //0 to 255
  int sat = 255; //0 to 255
  int lig = 128; //0 to 255
  int state = 0; //0 = hue, 1 = sat, 2 = lig
  
  Serial.println("Now slide for Hue!");
  while(state < 3){
  //change to sat/val by tap recognition
    pulse_data = read_reg(PULSE_SRC);
    if(pulse_data & 0x80){
      tap_detect = process_pulse(pulse_data);
    }
    if(tap_detect == 1){
      Serial.println("Next");
      state++;
      tap_detect = 0;
    }
    tslider.read();
    pos = tslider.getSliderPosition(); //ranges roughly from 22 to 70
    if(pos != -1){
      pos -= 22; //get rid of offset
      if(state == 0){
        hue = pos / 52.0 * 255;
        if(hue < 0){
          hue = 0;
        }
        if(hue > 255){
          hue = 255;
        }
        //Serial.println(hue);
      }
      else if(state == 1){
        sat = pos / 52.0 * 255;
        if(sat < 0){
          sat = 0;
        }
        if(sat > 255){
          sat = 255;
        }
        //Serial.println(sat);
      }
      else{
        lig = pos / 52.0 * 255;
        if(lig < 0){
          lig = 0;
        }
        if(lig > 255){
          lig = 255;
        }
        //Serial.println(lig);
      }
      change_color_hsl(hue, sat, lig);
    }
    delay(20);
  }
  analogWrite(LED_G, 255);
  analogWrite(LED_R, 255);
  analogWrite(LED_B, 0);
  delay(100);
  analogWrite(LED_R, 0);
  analogWrite(LED_G, 0);
  delay(50);
  analogWrite(LED_G, 255);
  analogWrite(LED_R, 255);
  delay(100);
  analogWrite(LED_R, 0);
  analogWrite(LED_G, 0);
  Serial.println("Done!");
}

/* Takes HSV parameters, transforms them to RGB and sends the result
 * to the associated lamp.
 */
void change_color_hsl(int hue, int sat, int lig){
  Serial.print("HSL: ");
  Serial.print(hue);
  Serial.print(sat);
  Serial.println(lig);
  int r, g, b;
  HSL_to_RGB(hue, sat, lig, &r, &g, &b);
  
  red = r;
  green = g;
  blue = b;
  send_to_lamp();
}

/* taken from http://qscribble.blogspot.de/2008/06/integer-conversion-from-hsl-to-rgb.html */
void HSL_to_RGB(int hue, int sat, int lum, int* r, int* g, int* b)
{
    int v;

    v = (lum < 128) ? (lum * (256 + sat)) >> 8 :
          (((lum + sat) << 8) - lum * sat) >> 8;
    if (v <= 0) {
        *r = *g = *b = 0;
    } else {
        int m;
        int sextant;
        int fract, vsf, mid1, mid2;

        m = lum + lum - v;
        hue *= 6;
        sextant = hue >> 8;
        fract = hue - (sextant << 8);
        vsf = v * fract * (v - m) / v >> 8;
        mid1 = m + vsf;
        mid2 = v - vsf;
        switch (sextant) {
           case 0: *r = v; *g = mid1; *b = m; break;
           case 1: *r = mid2; *g = v; *b = m; break;
           case 2: *r = m; *g = v; *b = mid1; break;
           case 3: *r = m; *g = mid2; *b = v; break;
           case 4: *r = mid1; *g = m; *b = v; break;
           case 5: *r = v; *g = m; *b = mid2; break;
        }
    }
}

void send_to_lamp(){
  //make sure all values have 3 digits and pad if necessary.
  Serial.print("LAMP: ");
  if(red < 100){
    if(red < 10){
      Serial.print("0");
    }
    Serial.print("0");
  }
  Serial.print(red);
  if(green < 100){
    if(green < 10){
      Serial.print("0");
    }
    Serial.print("0");
  }
  Serial.print(green);
  if(blue < 100){
    if(blue < 10){
      Serial.print("0");
    }
    Serial.print("0");
  }
  Serial.println(blue);
}

/* The usual setup stuff, setting pins, configuring the accelerometer
 * and starting serial communication.
 */
void setup(){
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(BT_WAKE_HW, OUTPUT);
  
  digitalWrite(BT_WAKE_HW, HIGH);
  Serial.begin(9600);
  Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_INT, I2C_RATE_800);
  bluetooth.begin(2400);
  
  //config stuff
  write_reg(CTRL_REG1, 0x00); //to clear previous config
  //write_reg(XYZ_DATA_CFG, HPF_EN_MASK); //output high-pass filtered data
  
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
  
  for(int i = 0; i < 7; i++){
    touch_state[i] = 0;
  }
  //start with white
  red = 100;
  green = 100;
  blue = 100;
  last_red = 100;
  last_green = 100;
  last_blue = 100;
  send_to_lamp();
}


void loop(){
  //need to save this because data is reset after reading
  uint8_t pulse_data = read_reg(PULSE_SRC);
  
  if(pulse_data & 0x80){
    process_pulse(pulse_data);
  }
  
  /* bluetooth stuff, doesn't work yet */
  if (bluetooth.available())
    Serial.write(bluetooth.read());
  if (Serial.available())
    bluetooth.write(Serial.read());
  
  
  /* capacitive touch stuff */  
  tslider.read();

  for(int i = 0; i < 7; i++){
    int val = tslider.getValue(i);
    if(val > 200){
      //touch at i
      //Serial.print("Touch at ");
      //Serial.println(i);
      touch_state[i]++;
    }
    else if(touch_state[i] > 0){ //touch released
      touch_state[i] = 0;
    }
  }
  
  //check for "cover touch"
  int segments_covered = 0;
  for(int i = 0; i < 7; i++){
    if(touch_state[i] > SEGMENT_COVER_THRESH){
      segments_covered++;
    }
  }
  if(segments_covered >= 6){
    process_rotation(); // <---!
    for(int i = 0; i < 7; i++){
      touch_state[i] = 1;
    }
  }

  //check for "mood change touch"
  if((touch_state[1] > 2*SEGMENT_COVER_THRESH) && (touch_state[2] > 2*SEGMENT_COVER_THRESH) && (touch_state[0] < SEGMENT_COVER_THRESH) && (touch_state[3] < SEGMENT_COVER_THRESH)){ //mood segment right
    Serial.println("Mood right");
    process_mood_change(1);
  }
  else if((touch_state[4] > 2*SEGMENT_COVER_THRESH) && (touch_state[5] > 2*SEGMENT_COVER_THRESH) && (touch_state[3] < SEGMENT_COVER_THRESH) && (touch_state[6] < SEGMENT_COVER_THRESH)){ //mood segment left
    Serial.println("Mood left");
    process_mood_change(-1);
  }
  
  //check for hue change activation touch (middle segment)
  if((touch_state[3] > 2*SEGMENT_COVER_THRESH) && (touch_state[2] < SEGMENT_COVER_THRESH) && (touch_state[4] < SEGMENT_COVER_THRESH)){
    Serial.println("Hue");
    process_hue_change();
  }
  delay(10);
}
