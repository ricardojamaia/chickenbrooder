#include "InputManager.h"
#include "DebugBrooder.h" // Include the debug macro

InputManager::InputManager(uint8_t increasePin, uint8_t decreasePin, StateValue *stateValue, unsigned long debounceDelay)
    : increasePin(increasePin), decreasePin(decreasePin), stateValue(stateValue), debounceDelay(debounceDelay), lastPressTime(0) {}

void InputManager::begin() {
  pinMode(increasePin, INPUT);
  pinMode(decreasePin, INPUT);
  DEBUG_BROODER_PRINTLN("InputManager initialized");
}

void InputManager::update() {
  unsigned long currentTime = millis();

  if (isButtonPressed(increasePin) && (currentTime - lastPressTime > debounceDelay)) {
    stateValue->increase(0.1);
    lastPressTime = currentTime;
    DEBUG_BROODER_PRINT("Value increased: ");
    DEBUG_BROODER_PRINTLN(stateValue->getValue());
  }

  if (isButtonPressed(decreasePin) && (currentTime - lastPressTime > debounceDelay)) {
    stateValue->decrease(0.1);
    lastPressTime = currentTime;
    DEBUG_BROODER_PRINT("Value decreased: ");
    DEBUG_BROODER_PRINTLN(stateValue->getValue());
  }
}

bool InputManager::isButtonPressed(uint8_t pin) {

  return digitalRead(pin) == HIGH; // Button press is HIGH

}