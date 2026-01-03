#include "Thermostat.h"
#include "BrooderLog.h"

Thermostat::Thermostat(State<float>* temperature, Lamp& heatingLamp1, Lamp& heatingLamp2, State<float>* targetTemperature)
    : temperature(temperature), heatingLamp1(heatingLamp1), heatingLamp2(heatingLamp2), targetTemperature(targetTemperature), useLamp1(true) {}

void Thermostat::begin() {

}

void Thermostat::update() {
  
  float lowThreshold = targetTemperature->getValue() - 1.0f; // 1 degree below the target temperature

  if (temperature->getValue() >= targetTemperature->getValue() && (heatingLamp1.getStatus() || heatingLamp2.getStatus())) {
    heatingLamp1.turnOff();
    heatingLamp2.turnOff();
    BROODER_LOG_D("Turning off both lamps.");

    BROODER_LOG_D("Switching to lamp %s", useLamp1 ? "2" : "1");
    useLamp1 = !useLamp1; // Toggle the lamp to use next time    
    return;
  }

  if (temperature->getValue() >= lowThreshold && 
    temperature->getValue() < targetTemperature->getValue() - 0.5f &&
    !heatingLamp1.getStatus() &&
    !heatingLamp2.getStatus()) {

    if (useLamp1) {
      heatingLamp1.turnOn();
      BROODER_LOG_D("Turning on lamp 1.");
    } else {
      heatingLamp2.turnOn();
      BROODER_LOG_D("Turning on lamp 2.");
    }

    return;
  }

  if (temperature->getValue() < lowThreshold && (!heatingLamp1.getStatus() || !heatingLamp2.getStatus())) {
    heatingLamp1.turnOn();
    heatingLamp2.turnOn();
    BROODER_LOG_D("Turning on both lamps.");

    return;
  }
}