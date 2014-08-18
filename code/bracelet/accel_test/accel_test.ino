#define ZPin 23
#define YPin 22
#define XPin 21
#define slpPin 12
#define gselPin 11

#define SIXGMODE 0

float x, y, z;
float mVperG;

void setup() {
  pinMode(slpPin, OUTPUT);
  pinMode(gselPin, OUTPUT);
  
  //prevent sleep mode
  digitalWrite(slpPin, HIGH);
  
  //configure sensitivity (1.5g or 6g)
  if(SIXGMODE){
    digitalWrite(gselPin, HIGH);
    mVperG = 0.206;
  }
  else{
    digitalWrite(gselPin, LOW);
    mVperG = 0.8;
  }
  
  Serial.begin(115200);
  Serial.println("Serial ready");
}

void loop() {
  //analog output is between 0 (0V) and 1023 (5V) -> 0.0049V per unit
  //sensor outputs voltages centered ar vdd/2 -> 3.3/2 -> 1.65 -> 336.5 analogRead
  //sensor outputs will always be within range of 0 to VDD -> 0 to 673
  //1g -> 800mV if gsel = low
  //1g -> 206mV if gsel = high
  
  x = ((analogRead(XPin) * 0.0049) - 1.65) / mVperG;
  y = ((analogRead(YPin) * 0.0049) - 1.65) / mVperG;
  z = ((analogRead(ZPin) * 0.0049) - 1.65) / mVperG;
  
  Serial.print("X: ");
  Serial.print(x);
  Serial.print(" Y: ");
  Serial.print(y);
  Serial.print(" Z: ");
  Serial.println(z);
  //Serial.println(((analogRead(ZPin) * 0.0049) - 1.65) / 0.8);
  delay(100);
}
  
