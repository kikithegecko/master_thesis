#include <SoftwareSerial.h>

#define rxPin 2
#define txPin 4

#define RPin 5
#define GPin 6
#define BPin 3

int red = 0;
int green = 0;
int blue = 125;

String inputString = ""; // for incoming data
boolean stringComplete = false;

SoftwareSerial bluetooth(rxPin, txPin);

void setup() {
  pinMode(RPin, OUTPUT);
  pinMode(GPin, OUTPUT);
  pinMode(BPin, OUTPUT);
  
  Serial.begin(9600);
  Serial.println("Serial ready");
  
  //taken from https://wiki.hci.uni-hannover.de/doku.php?id=research:thesis:ems-navigation:prototyping
  bluetooth.begin(115200);        // The Bluetooth Mate defaults to 115200bps
  delay(320);                     // IMPORTANT DELAY! (Minimum ~276ms)
  bluetooth.print("$$$");         // Enter command mode
  delay(15);                      // IMPORTANT DELAY! (Minimum ~10ms)
  bluetooth.println("U,9600,N");  // Temporarily Change the baudrate to 9600, no parity
  bluetooth.begin(9600);          // Start bluetooth serial at 9600
  
  bluetooth.println("Bluetooth ready");
}

void loop() {
  
  if(stringComplete) {
    // protocol is "rrrgggbbb", so check length and syntax
    if(inputString.length() != 10){
        bluetooth.println("Error! Wrong String length!");
    }
    else { // decompose and convert
      red = (String(inputString[0]) + String(inputString[1]) + String(inputString[2])).toInt();
      green = (String(inputString[3]) + String(inputString[4]) + String(inputString[5])).toInt();
      blue = (String(inputString[6]) + String(inputString[7]) + String(inputString[8])).toInt();
        
      Serial.print("R: ");
      Serial.print(red);
      Serial.print(" G: ");
      Serial.print(green);
      Serial.print(" B: ");
      Serial.println(blue);
    }
    inputString = "";
    stringComplete = false;
  }
  
  analogWrite(RPin, red);
  analogWrite(GPin, green);
  analogWrite(BPin, blue);
  
  if(bluetooth.available()) {
    char inChar = bluetooth.read();
    inputString += inChar;
    if(inChar == '\n') {
      stringComplete = true;
    }
  }
}
