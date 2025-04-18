#include "Sensor.h"

Sensor::Sensor(uint8_t pin, uint8_t type, State<float>* temperature, State<float>* humidity)
    : dht(pin, type), temperature(temperature), humidity(humidity) {}

void Sensor::begin() {
  dht.begin();
}

void Sensor::update() {
  float currentTemperature = dht.readTemperature();
  float currentHumidity = dht.readHumidity();

  if (isnan(currentTemperature) || isnan(currentHumidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Update the StateValue objects
  temperature->setValue(currentTemperature);
  humidity->setValue(currentHumidity);
}