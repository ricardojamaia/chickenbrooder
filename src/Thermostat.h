#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include <Arduino.h>
#include <DHT.h>
#include "StateValue.h" // Include the StateValue interface

class Thermostat {
public:
  Thermostat(uint8_t dhtPin, uint8_t dhtType, uint8_t lamp1Pin, uint8_t lamp2Pin, StateValue *lowThreshold, StateValue *highThreshold);
  void update();
  float getTemperature();
  float getHumidity();

private:
  uint8_t lamp1Pin;
  uint8_t lamp2Pin;
  StateValue *lowThreshold; // Use StateValue for low threshold
  StateValue *highThreshold; // Use StateValue for high threshold
  bool useLamp1;
  bool bothLampsOn;

  DHT dht; // DHT sensor instance
  float currentTemperature;
  float currentHumidity;

  void controlLamps(float temperature);
};

#endif