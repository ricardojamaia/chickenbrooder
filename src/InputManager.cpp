#include "InputManager.h"
#include "BrooderLog.h" // Include the debug macro

InputManager::InputManager(uint8_t increasePin, uint8_t decreasePin, State<float>* targetTemperature, unsigned long debounceDelay)
    : increasePin(increasePin), decreasePin(decreasePin), targetTemperature(targetTemperature), debounceDelay(debounceDelay), lastPressTime(0) {}

void InputManager::begin() {
  pinMode(increasePin, INPUT);
  pinMode(decreasePin, INPUT);
  BROODER_LOG_D("InputManager initialized");
}

void InputManager::update() {
  unsigned long currentTime = millis();

  if (isButtonPressed(increasePin) && (currentTime - lastPressTime > debounceDelay)) {
    targetTemperature->increase(0.1);
    lastPressTime = currentTime;
    BROODER_LOG_D("Value increased: %.2f", targetTemperature->getValue());
  }

  if (isButtonPressed(decreasePin) && (currentTime - lastPressTime > debounceDelay)) {
    targetTemperature->decrease(0.1);
    lastPressTime = currentTime;
    BROODER_LOG_D("Value decreased: %.2f", targetTemperature->getValue());
  }
}

bool InputManager::isButtonPressed(uint8_t pin) {

  return digitalRead(pin) == HIGH; // Button press is HIGH

}