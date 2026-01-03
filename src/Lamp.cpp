#include "Lamp.h"
#include <esp_log.h>

Lamp::Lamp(uint8_t pin) : pin(pin), isOn(false) {}

void Lamp::begin() {
  pinMode(pin, OUTPUT);    // Set the pin as an output
  digitalWrite(pin, LOW);  // Initialize the lamp as OFF
}

void Lamp::turnOn() {
  if (!isOn) {
    log_d("Turning ON lamp on pin: %u", pin);
    digitalWrite(pin, HIGH); // Turn the lamp ON
    isOn = true;
  }
}

void Lamp::turnOff() {
  if (isOn) {
    log_d("Turning OFF lamp on pin: ", pin);
    digitalWrite(pin, LOW); // Turn the lamp OFF
    isOn = false;
  }
}

bool Lamp::getStatus() const {
  return isOn;
}