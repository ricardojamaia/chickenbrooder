#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <Arduino.h>
#include "State.h"

class PushButton {
public:
  PushButton(uint8_t pin, State<bool>* state);

  void begin();
  void update();

private:
  uint8_t pin;
  State<bool>* state;
  bool lastButtonState;
};

#endif // PUSHBUTTON_H