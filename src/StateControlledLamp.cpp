#include "StateControlledLamp.h"

StateControlledLamp::StateControlledLamp(uint8_t pin, State<bool>* state) : Lamp(pin), state(state) {}

void StateControlledLamp::begin() {
  Lamp::begin();

  // Register the lamp as a listener to the state
  state->addListener(onStateChange, this);
}

void StateControlledLamp::onStateChange(bool newState, void* context) {
  StateControlledLamp* lamp = static_cast<StateControlledLamp*>(context);
  if (newState) {
    lamp->turnOn();
  } else {
    lamp->turnOff();
  }
}

void StateControlledLamp::turnOn() {
  if (!isOn) {
    Lamp::turnOn();
    state->setValue(true); // Update the state to ON
  }
}

void StateControlledLamp::turnOff() {
  if (isOn) {
    Lamp::turnOff();
    state->setValue(false); // Update the state to OFF
  }
}