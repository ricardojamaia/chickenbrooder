#ifndef THERMOSTAT_H
#define THERMOSTAT_H


#include "Lamp.h"
#include "State.h"
#include "Sensor.h"

class Thermostat {
public:
  Thermostat(State<float>* temperature, Lamp& heatingLamp1, Lamp& heatingLamp2, State<float>* targetTemperature);

  void begin();

  void update();

private:
  State<float>* temperature;      // Reference to the current temperature
  Lamp& heatingLamp1;                  // Reference to Lamp 1
  Lamp& heatingLamp2;                  // Reference to Lamp 2
  State<float>* targetTemperature; // Reference to the target temperature
  bool useLamp1;                // Flag to alternate between lamps

};

#endif // THERMOSTAT_H