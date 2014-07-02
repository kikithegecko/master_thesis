#include <SoftwareSerial.h>

#define rxPin 0
#define txPin 1

#define RPin 5
#define GPin 6
#define BPin 3

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
  analogWrite(RPin, 0);
  analogWrite(GPin, 0);
  analogWrite(BPin, 255);
}
