//Pin configurations
#define ZPin 23
#define YPin 22
#define XPin 21
#define slpPin 12
#define gselPin 11
#define buttonPin 14

#define RPin 5
#define GPin 4
#define BPin 6

//application configurations
#define SIXGMODE 0
#define SAMPLE_SIZE 100 //number of samples per Gesture
#define DELAY_SIZE 10 //delay betweeen two samples

typedef struct {
  float x;
  float y;
  float z;
} AccelData;

AccelData gesture[SAMPLE_SIZE];
float x, y, z;
float mVperG;
int buttonstate = 0;

void setup() {
  pinMode(slpPin, OUTPUT);
  pinMode(gselPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  
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

void readAccel(){
  //read 
  
  //analog output is between 0 (0V) and 1023 (5V) -> 0.0049V per unit
  //sensor outputs voltages centered ar vdd/2 -> 3.3/2 -> 1.65 -> 336.5 analogRead
  //sensor outputs will always be within range of 0 to VDD -> 0 to 673
  //1g -> 800mV if gsel = low
  //1g -> 206mV if gsel = high
  for(int i = 0; i < SAMPLE_SIZE; i++){
  
    x = ((analogRead(XPin) * 0.0049) - 1.65) / mVperG;
    y = ((analogRead(YPin) * 0.0049) - 1.65) / mVperG;
    z = ((analogRead(ZPin) * 0.0049) - 1.65) / mVperG;
  
    //Serial.print("X: ");
    Serial.print(x);
    //Serial.print(" Y: ");
    Serial.print(" ");
    Serial.print(y);
    //Serial.print(" Z: ");
    Serial.print(" ");
    Serial.println(z);
    //Serial.println(((analogRead(ZPin) * 0.0049) - 1.65) / 0.8);
    
    gesture[i].x = x;
    gesture[i].y = y;
    gesture[i].z = z;
    
    delay(DELAY_SIZE);
  }
}

void loop() {
  analogWrite(RPin, 255);
  analogWrite(GPin, 255);
  analogWrite(BPin, 255);
  
  buttonstate = digitalRead(buttonPin);
  if(buttonstate == HIGH){
    //Serial.println("Button pressed!");
    readAccel();
  }
}
  
