#include "Thermostat.h"
#include "DebugBrooder.h"

Thermostat::Thermostat(StateValue* temperature, Lamp& lamp1, Lamp& lamp2, StateValue* targetTemperature)
    : temperature(temperature), lamp1(lamp1), lamp2(lamp2), targetTemperature(targetTemperature), useLamp1(true) {}

void Thermostat::begin() {

}

void Thermostat::update() {
  
  float lowThreshold = targetTemperature->getValue() * 0.9;


  if (temperature->getValue() < lowThreshold) {
    lamp1.turnOn();
    lamp2.turnOn();
  } else if (temperature->getValue() >= lowThreshold && temperature->getValue() < targetTemperature->getValue()) {
    if (useLamp1) {
      lamp1.turnOn();
      lamp2.turnOff();
    } else {
      lamp1.turnOff();
      lamp2.turnOn();
    }
  } else if (temperature->getValue() > targetTemperature->getValue()) {
    if (lamp1.getStatus() || lamp2.getStatus()) {
      lamp1.turnOff();
      lamp2.turnOff();

      DEBUG_BROODER_PRINT("Switching to lamp ");
      DEBUG_BROODER_PRINTLN(useLamp1 ? "2" : "1");
      useLamp1 = !useLamp1; // Toggle the lamp to use next time
    }
  }
}