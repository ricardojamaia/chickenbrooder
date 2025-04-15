#include "Thermostat.h"

Thermostat::Thermostat(uint8_t dhtPin, uint8_t dhtType, uint8_t lamp1Pin, uint8_t lamp2Pin, StateValue *lowThreshold, StateValue *highThreshold, StateValue *temperature, StateValue *humidity)
    : dht(dhtPin, dhtType), lamp1Pin(lamp1Pin), lamp2Pin(lamp2Pin), lowThreshold(lowThreshold), highThreshold(highThreshold), temperature(temperature), humidity(humidity), useLamp1(true), bothLampsOn(false) {
  pinMode(lamp1Pin, OUTPUT);
  pinMode(lamp2Pin, OUTPUT);
  digitalWrite(lamp1Pin, LOW);
  digitalWrite(lamp2Pin, LOW);
  dht.begin();
}

void Thermostat::update() {
  float currentTemperature = dht.readTemperature();
  float currentHumidity = dht.readHumidity();

  if (isnan(currentTemperature) || isnan(currentHumidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Update the StateValue objects
  temperature->setValue(currentTemperature);
  humidity->setValue(currentHumidity);

  // Control the lamps based on the current temperature
  controlLamps(currentTemperature);
}

void Thermostat::controlLamps(float temperature) {
  if (temperature < lowThreshold->getValue()) {
    digitalWrite(lamp1Pin, HIGH);
    digitalWrite(lamp2Pin, HIGH);
    bothLampsOn = true;
  } else if (temperature >= lowThreshold->getValue() && temperature < highThreshold->getValue()) {
    if (bothLampsOn) {
      digitalWrite(lamp1Pin, LOW);
      digitalWrite(lamp2Pin, LOW);
      bothLampsOn = false;
      delay(1000);
    }

    if (useLamp1) {
      digitalWrite(lamp1Pin, HIGH);
      digitalWrite(lamp2Pin, LOW);
    } else {
      digitalWrite(lamp1Pin, LOW);
      digitalWrite(lamp2Pin, HIGH);
    }
    useLamp1 = !useLamp1;
  } else {
    digitalWrite(lamp1Pin, LOW);
    digitalWrite(lamp2Pin, LOW);
    bothLampsOn = false;
  }
}