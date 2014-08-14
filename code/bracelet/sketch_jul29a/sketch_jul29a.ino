#include "TouchSlider.h"

//const int TOUCH_PINS[] = {23, 19, 22, 18, 15, 17, 16};
const int TOUCH_PINS[] = {22, 23, 19, 17, 15, 18, 16};


TouchSlider slider(7, TOUCH_PINS);

void setup() {
  Serial.begin(115200);
}

void loop() {
  /*
  int maxVal = -1;
  int maxID = -1;
  
  for(int i = 0; i < 7; ++i) {
    int value = touchRead(TOUCH_PINS[i]);
    if(value > maxVal) {
      maxVal = value;
      maxID = i;
    }
    Serial.print(value);
    Serial.print(" ");
  }
  
  Serial.print(": ");
  if(maxVal > 1300) {
    Serial.println(maxID);
  } else {
    Serial.println("--");
  }
 
  */
  slider.read();
/*
  for(int i = 0; i < 7; ++i) {
    Serial.print(slider.getValue(i));
    Serial.print(" ");
  }
  
  Serial.println(slider.isActive());
*/

  int sliderPos = slider.getSliderPosition();
  for(int i = 0; i < sliderPos; ++i) {
    Serial.print("*");
  }
  Serial.println("");

  delay(50);
}
