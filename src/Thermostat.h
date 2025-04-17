#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include <Arduino.h>
#include <DHT.h>
#include "Lamp.h"
#include "StateValue.h"

class Thermostat {
public:
  Thermostat(uint8_t dhtPin, uint8_t dhtType, Lamp& lamp1, Lamp& lamp2, StateValue* targetTemperature, StateValue* temperature, StateValue* humidity);
  void begin(); // Initialization method
  void update();

private:
  DHT dht;
  Lamp& lamp1; // Reference to Lamp 1
  Lamp& lamp2; // Reference to Lamp 2

  StateValue* targetTemperature;
  StateValue* temperature;
  StateValue* humidity;
  bool useLamp1; // Flag to alternate between lamps

  void controlLamps(float temperature);
};

#endif // THERMOSTAT_H