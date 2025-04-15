#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include <Arduino.h>
#include <DHT.h>
#include "StateValue.h"

class Thermostat {
public:
  Thermostat(uint8_t dhtPin, uint8_t dhtType, uint8_t lamp1Pin, uint8_t lamp2Pin, StateValue *lowThreshold, StateValue *highThreshold, StateValue *temperature, StateValue *humidity);
  void update();
  float getTemperature();
  float getHumidity();

private:
  uint8_t lamp1Pin;
  uint8_t lamp2Pin;
  StateValue *lowThreshold;
  StateValue *highThreshold;
  StateValue *temperature;
  StateValue *humidity;
  bool useLamp1;
  bool bothLampsOn;

  DHT dht; // DHT sensor instance
  float currentTemperature;
  float currentHumidity;

  void controlLamps(float temperature);
};

#endif