#include "Lamp.h"
#include "DebugBrooder.h"

Lamp::Lamp(uint8_t pin) : pin(pin), isOn(false) {}

void Lamp::begin() {
  pinMode(pin, OUTPUT);    // Set the pin as an output
  digitalWrite(pin, LOW);  // Initialize the lamp as OFF
}

void Lamp::turnOn() {
  if (!isOn) {
    DEBUG_BROODER_PRINTLN("Turning ON lamp on pin: " + String(pin));
    digitalWrite(pin, HIGH); // Turn the lamp ON
    isOn = true;
  }
}

void Lamp::turnOff() {
  if (isOn) {
    DEBUG_BROODER_PRINTLN("Turning OFF lamp on pin: " + String(pin));
    digitalWrite(pin, LOW); // Turn the lamp OFF
    isOn = false;
  }
}

bool Lamp::getStatus() const {
  return isOn;
}