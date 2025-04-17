#include "Thermostat.h"
#include "DebugBrooder.h"

Thermostat::Thermostat(uint8_t dhtPin, uint8_t dhtType, Lamp& lamp1, Lamp& lamp2, StateValue* targetTemperature, StateValue* temperature, StateValue* humidity)
    : dht(dhtPin, dhtType), lamp1(lamp1), lamp2(lamp2), targetTemperature(targetTemperature), temperature(temperature), humidity(humidity), useLamp1(true) {
    }

void Thermostat::begin() {
  lamp1.begin();
  lamp2.begin();
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
  
  float lowThreshold = targetTemperature->getValue() * 0.9;
  
  if (temperature < lowThreshold) {
    lamp1.turnOn();
    lamp2.turnOn();
  } else if (temperature >= lowThreshold && temperature < targetTemperature->getValue()) {
    if (useLamp1) {
      lamp1.turnOn();
      lamp2.turnOff();
    } else {
      lamp1.turnOff();
      lamp2.turnOn();
    }
  } else if (temperature > targetTemperature->getValue()){
    if (lamp1.getStatus() || lamp2.getStatus()) {
      lamp1.turnOff();
      lamp2.turnOff();

      DEBUG_BROODER_PRINT("Switching to lamp ");
      DEBUG_BROODER_PRINTLN(useLamp1 ? "2" : "1");
      useLamp1 = !useLamp1; // Toggle the lamp to use next time
    }
  }
}