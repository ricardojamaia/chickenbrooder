#include "StateControlledLamp.h"

StateControlledLamp::StateControlledLamp(uint8_t pin, State<bool>* state) : Lamp(pin), state(state) {}

void StateControlledLamp::begin() {
  Lamp::begin();

  // Register the lamp as a listener to the state
  state->addListener([this](bool newState) {
    if (newState) {
      turnOn();
    } else {
      turnOff();
    }
  });
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