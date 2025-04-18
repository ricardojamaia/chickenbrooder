#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <DHT.h>
#include "State.h"

class Sensor {
public:
  Sensor(uint8_t pin, uint8_t type, State<float>* temperature, State<float>* humidity);

  void begin();
  void update();

private:
  DHT dht;
  State<float>* temperature; // Reference to the temperature StateValue
  State<float>* humidity;    // Reference to the humidity StateValue
};

#endif // SENSOR_H