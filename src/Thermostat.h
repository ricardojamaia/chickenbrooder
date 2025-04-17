#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include <Arduino.h>
#include "Lamp.h"
#include "StateValue.h"
#include "Sensor.h"

class Thermostat {
public:
  Thermostat(StateValue* temperature, Lamp& heatingLamp1, Lamp& heatingLamp2, StateValue* targetTemperature);

  void begin();

  void update();

private:
  StateValue* temperature;      // Reference to the current temperature
  Lamp& heatingLamp1;                  // Reference to Lamp 1
  Lamp& heatingLamp2;                  // Reference to Lamp 2
  StateValue* targetTemperature; // Reference to the target temperature
  bool useLamp1;                // Flag to alternate between lamps

};

#endif // THERMOSTAT_H