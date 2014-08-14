#define ZPin 23
#define YPin 22
#define XPin 21
#define slpPin 12

void setup() {
  //pinMode(ZPin, INPUT);
  //pinMode(YPin, INPUT);
  //pinMode(XPin, INPUT);
  pinMode(slpPin, OUTPUT);
  
  //prevent sleep mode
  digitalWrite(slpPin, HIGH);
  
  Serial.begin(115200);
  Serial.println("Serial ready");
}

void loop() {
  //analog output is between 0 (0V) and 1023 (5V) -> 0.0049V per unit
  //sensor outputs voltages centered ar vdd/2 -> 3.3/2 -> 1.65 -> 336.5 analogRead
  //sensor outputs will always be within range of 0 to VDD -> 0 to 673
  //1g -> 800mV if gsel = low
  
  Serial.print("X: ");
  Serial.print(((analogRead(XPin) * 0.0049) - 1.65) / 0.8);
  Serial.print(" Y: ");
  Serial.print(((analogRead(YPin) * 0.0049) - 1.65) / 0.8);
  Serial.print(" Z: ");
  Serial.println(((analogRead(ZPin) * 0.0049) - 1.65) / 0.8);
  delay(50);
}
  
