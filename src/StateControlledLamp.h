#ifndef STATECONTROLLEDLAMP_H
#define STATECONTROLLEDLAMP_H

#include "Lamp.h"
#include "State.h"

class StateControlledLamp : public Lamp {
public:
  StateControlledLamp(uint8_t pin, State<bool>* state);

  void begin() override;

private:
  State<bool>* state;

  static void onStateChange(bool newState, void* context);
};

#endif // STATECONTROLLEDLAMP_H