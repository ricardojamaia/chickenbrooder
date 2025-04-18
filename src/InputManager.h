#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <Arduino.h>
#include "State.h"

class InputManager {
public:
  InputManager(uint8_t increasePin, uint8_t decreasePin, State<float>* targetTemperature, unsigned long debounceDelay = 200);
  void begin();
  void update();

private:
  uint8_t increasePin;
  uint8_t decreasePin;
  State<float> *targetTemperature;
  unsigned long debounceDelay;
  unsigned long lastPressTime;

  bool isButtonPressed(uint8_t pin);
};

#endif // INPUT_MANAGER_H