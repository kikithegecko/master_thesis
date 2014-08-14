#pragma once

#include <cstring>
#include <climits>

#include <core_pins.h>

class TouchSlider {
public:
  TouchSlider(int pinCount, const int * pins) : _pinCount(pinCount), _pins(pins) { 
    _baselines = new int[pinCount];
    for(int i = 0; i < pinCount; ++i) {
      _baselines[i] = USHRT_MAX;
    }
    _values = new int[pinCount];
  }
  
  ~TouchSlider() {
    delete[] _baselines;
    delete[] _values;
  }
  
  void read() {
    for(int i = 0; i < _pinCount; ++i) {
      _values[i] = touchRead(_pins[i]);
      if(_values[i] < _baselines[i]) {
        _baselines[i] = _values[i];
      }
    }
  }
  
  bool isActive() {
    for(int i = 0; i < _pinCount; ++i) {
      if(_values[i] - _baselines[i] > 300) {
        return true;
      }
    }
    return false;
  }
  
  int getBaseline(int pin) {
    return _baselines[pin];
  }
  
  int getRawValue(int pin) {
    return _values[pin];
  }
  
  int getValue(int pin) {
    return _values[pin] - _baselines[pin];
  }
  
  int getSliderPosition() {
    int sumA = 0;
    int sumB = 0;
    for(int i = 0; i < _pinCount; ++i) {
      sumA += getValue(i);
      sumB += (i + 1) * getValue(i);
    }
    
    if(sumA < 500) {
      return -1;
    }
       
    return (int)(100.0f * sumB / sumA / (_pinCount + 1.0f));
  }
  
private:
  const int _pinCount;
  const int * _pins;
  int * _baselines;
  int * _values;
};
