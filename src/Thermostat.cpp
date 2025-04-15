#include "Thermostat.h"

Thermostat::Thermostat(uint8_t dhtPin, uint8_t dhtType, uint8_t lamp1Pin, uint8_t lamp2Pin, StateValue *lowThreshold, StateValue *highThreshold)
    : dht(dhtPin, dhtType), lamp1Pin(lamp1Pin), lamp2Pin(lamp2Pin), lowThreshold(lowThreshold), highThreshold(highThreshold), useLamp1(true), bothLampsOn(false), currentTemperature(NAN), currentHumidity(NAN) {
  pinMode(lamp1Pin, OUTPUT);
  pinMode(lamp2Pin, OUTPUT);
  digitalWrite(lamp1Pin, LOW);
  digitalWrite(lamp2Pin, LOW);
  dht.begin();
}

void Thermostat::update() {
  currentTemperature = dht.readTemperature();
  currentHumidity = dht.readHumidity();

  if (isnan(currentTemperature) || isnan(currentHumidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  controlLamps(currentTemperature);
}

void Thermostat::controlLamps(float temperature) {
  if (temperature < lowThreshold->getValue()) {
    // Turn on both lamps if temperature is too low
    digitalWrite(lamp1Pin, HIGH);
    digitalWrite(lamp2Pin, HIGH);
    bothLampsOn = true;
  } else if (temperature >= lowThreshold->getValue() && temperature < highThreshold->getValue()) {
    // Turn off both lamps before alternating
    if (bothLampsOn) {
      digitalWrite(lamp1Pin, LOW);
      digitalWrite(lamp2Pin, LOW);
      bothLampsOn = false;
      delay(1000); // Ensure a delay before switching lamps
    }

    // Alternate between lamps
    if (useLamp1) {
      digitalWrite(lamp1Pin, HIGH);
      digitalWrite(lamp2Pin, LOW);
    } else {
      digitalWrite(lamp1Pin, LOW);
      digitalWrite(lamp2Pin, HIGH);
    }
    useLamp1 = !useLamp1; // Alternate for the next cycle
  } else {
    // Turn off both lamps if temperature is high
    digitalWrite(lamp1Pin, LOW);
    digitalWrite(lamp2Pin, LOW);
    bothLampsOn = false;
  }
}

float Thermostat::getTemperature() {
  return currentTemperature;
}

float Thermostat::getHumidity() {
  return currentHumidity;
}