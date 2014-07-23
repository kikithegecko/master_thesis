#include <SoftwareSerial.h>

#define rxPin 2
#define txPin 4

SoftwareSerial bluetooth(rxPin, txPin);

void setup(){
  Serial.begin(9600);
  Serial.println("Serial ready");
  
  bluetooth.begin(115200);
  delay(320);
  bluetooth.print("$$$"); //enter CMD mode
  delay(15);
  bluetooth.println("U,9600,N");
  bluetooth.begin(9600);
  bluetooth.println("Bluetooth ready");
  delay(320);
}

void loop(){
  for(int i = 0; i < 11500; i++){
    bluetooth.write(255);
  }
  delay(100);
}
