#include "Thermostat.h"
#include "DebugBrooder.h"

Thermostat::Thermostat(State<float>* temperature, Lamp& heatingLamp1, Lamp& heatingLamp2, State<float>* targetTemperature)
    : temperature(temperature), heatingLamp1(heatingLamp1), heatingLamp2(heatingLamp2), targetTemperature(targetTemperature), useLamp1(true) {}

void Thermostat::begin() {

}

void Thermostat::update() {
  
  float lowThreshold = targetTemperature->getValue() - 1.0f; // 1 degree below the target temperature

  if (temperature->getValue() >= targetTemperature->getValue()) {
    heatingLamp1.turnOff();
    heatingLamp2.turnOff();
    DEBUG_BROODER_PRINTLN("Turning off both lamps.");

    DEBUG_BROODER_PRINT("Switching to lamp ");
    DEBUG_BROODER_PRINTLN(useLamp1 ? "2" : "1");
    useLamp1 = !useLamp1; // Toggle the lamp to use next time    
    return;
  }

  if (temperature->getValue() >= lowThreshold && 
    temperature->getValue() < targetTemperature->getValue() - 0.5f &&
    !heatingLamp1.getStatus() &&
    !heatingLamp2.getStatus()) {

    if (useLamp1) {
      heatingLamp1.turnOn();
      DEBUG_BROODER_PRINTLN("Turning on lamp 1.");
    } else {
      heatingLamp2.turnOn();
      DEBUG_BROODER_PRINTLN("Turning on lamp 2.");
    }

    return;
  }

  if (temperature->getValue() < lowThreshold) {
    heatingLamp1.turnOn();
    heatingLamp2.turnOn();
    DEBUG_BROODER_PRINTLN("Turning on both lamps.");

    return;
  }
}