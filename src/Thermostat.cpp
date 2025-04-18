#include "Thermostat.h"
#include "DebugBrooder.h"

Thermostat::Thermostat(State<float>* temperature, Lamp& heatingLamp1, Lamp& heatingLamp2, State<float>* targetTemperature)
    : temperature(temperature), heatingLamp1(heatingLamp1), heatingLamp2(heatingLamp2), targetTemperature(targetTemperature), useLamp1(true) {}

void Thermostat::begin() {

}

void Thermostat::update() {
  
  float lowThreshold = targetTemperature->getValue() * 0.9;


  if (temperature->getValue() < lowThreshold) {
    heatingLamp1.turnOn();
    heatingLamp2.turnOn();
  } else if (temperature->getValue() >= lowThreshold && temperature->getValue() < targetTemperature->getValue()) {
    if (useLamp1) {
      heatingLamp1.turnOn();
      heatingLamp2.turnOff();
    } else {
      heatingLamp1.turnOff();
      heatingLamp2.turnOn();
    }
  } else if (temperature->getValue() > targetTemperature->getValue()) {
    if (heatingLamp1.getStatus() || heatingLamp2.getStatus()) {
      heatingLamp1.turnOff();
      heatingLamp2.turnOff();

      DEBUG_BROODER_PRINT("Switching to lamp ");
      DEBUG_BROODER_PRINTLN(useLamp1 ? "2" : "1");
      useLamp1 = !useLamp1; // Toggle the lamp to use next time
    }
  }
}