#define RPin 5
#define GPin 6
#define BPin 3

#define FADESPEED 15 // make this higher to slow down

int red = 0;
int green = 0;
int blue = 0;

void setup() {
  pinMode(RPin, OUTPUT);
  pinMode(GPin, OUTPUT);
  pinMode(BPin, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("Serial ready");
}

void loop() {
  int r, g, b;
  // fade from blue to violet
  for (r = 0; r < 256; r++) {
    analogWrite(RPin, r);
    delay(FADESPEED);
  }
  // fade from violet to red
  for (b = 255; b > 0; b--) {
    analogWrite(BPin, b);
  delay(FADESPEED);
  }
  // fade from red to yellow
  for (g = 0; g < 256; g++) {
    analogWrite(GPin, g);
    delay(FADESPEED);
  }
  // fade from yellow to green
  for (r = 255; r > 0; r--) {
    analogWrite(RPin, r);
    delay(FADESPEED);
  }
  // fade from green to teal
  for (b = 0; b < 256; b++) {
    analogWrite(BPin, b);
    delay(FADESPEED);
  }
  // fade from teal to blue
  for (g = 255; g > 0; g--) {
    analogWrite(GPin, g);
    delay(FADESPEED);
  }
}
