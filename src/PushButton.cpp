#include "PushButton.h"

PushButton::PushButton(uint8_t pin, State<bool>* state) : pin(pin), state(state), lastButtonState(false) {}

void PushButton::begin() {
  pinMode(pin, INPUT); // Configure the button pin with an internal pull-up resistor
}

void PushButton::update() {
  bool currentButtonState = digitalRead(pin) == HIGH; // Button pressed when LOW
  if (currentButtonState && !lastButtonState) {
    // Toggle the state when the button is pressed
    state->setValue(!state->getValue());
  }
  lastButtonState = currentButtonState;
}