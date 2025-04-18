#include "InputManager.h"
#include "DebugBrooder.h" // Include the debug macro

InputManager::InputManager(uint8_t increasePin, uint8_t decreasePin, State<float>* targetTemperature, unsigned long debounceDelay)
    : increasePin(increasePin), decreasePin(decreasePin), targetTemperature(targetTemperature), debounceDelay(debounceDelay), lastPressTime(0) {}

void InputManager::begin() {
  pinMode(increasePin, INPUT);
  pinMode(decreasePin, INPUT);
  DEBUG_BROODER_PRINTLN("InputManager initialized");
}

void InputManager::update() {
  unsigned long currentTime = millis();

  if (isButtonPressed(increasePin) && (currentTime - lastPressTime > debounceDelay)) {
    targetTemperature->increase(0.1);
    lastPressTime = currentTime;
    DEBUG_BROODER_PRINT("Value increased: ");
    DEBUG_BROODER_PRINTLN(targetTemperature->getValue());
  }

  if (isButtonPressed(decreasePin) && (currentTime - lastPressTime > debounceDelay)) {
    targetTemperature->decrease(0.1);
    lastPressTime = currentTime;
    DEBUG_BROODER_PRINT("Value decreased: ");
    DEBUG_BROODER_PRINTLN(targetTemperature->getValue());
  }
}

bool InputManager::isButtonPressed(uint8_t pin) {

  return digitalRead(pin) == HIGH; // Button press is HIGH

}