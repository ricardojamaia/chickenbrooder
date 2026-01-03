#include "Sensor.h"
#include "DebugBrooder.h"

#define REPORT_ERROR_EVERY_MILLIS 60 * 1000

Sensor::Sensor(uint8_t pin, uint8_t type, State<float>* temperature, State<float>* humidity)
    : dht(pin, type), temperature(temperature), humidity(humidity) {}

void Sensor::begin() {
  dht.begin();
}

void Sensor::update() {
  float currentTemperature = dht.readTemperature();
  float currentHumidity = dht.readHumidity();

  if (isnan(currentTemperature) || isnan(currentHumidity)) {
    unsigned long actual_milis = millis();
    m_num_errors++;
    if (m_num_errors == 1){
      DEBUG_BROODER_PRINTLN("Error reading DHT sensor.");
    } else if (m_lastReportedFailure + REPORT_ERROR_EVERY_MILLIS < actual_milis){
      m_lastReportedFailure = actual_milis;
      DEBUG_BROODER_PRINT("Error reading DHT sensor for the last ");
      DEBUG_BROODER_PRINT(m_lastReportedFailure);
      DEBUG_BROODER_PRINTLN(" times.");
    }
    return;
  }

  m_num_errors = 0;

  // Update the StateValue objects
  temperature->setValue(currentTemperature);
  humidity->setValue(currentHumidity);
}