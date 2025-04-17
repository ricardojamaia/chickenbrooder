#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <DHT.h>
#include "StateValue.h"

class Sensor {
public:
  Sensor(uint8_t pin, uint8_t type, StateValue* temperature, StateValue* humidity);

  void begin();
  void update();

private:
  DHT dht;
  StateValue* temperature; // Reference to the temperature StateValue
  StateValue* humidity;    // Reference to the humidity StateValue
};

#endif // SENSOR_H