#define RPin 5
#define GPin 6
#define BPin 3

void setup() {
  pinMode(RPin, OUTPUT);
  pinMode(GPin, OUTPUT);
  pinMode(BPin, OUTPUT);
}

void loop(){
  analogWrite(RPin, 0);
  analogWrite(GPin, 0);
  analogWrite(BPin, 0);
}
