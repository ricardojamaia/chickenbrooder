#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H
#include <PubSubClient.h>
#include <WiFi.h>
#include "State.h"

#define MQTT_LOGGER_TOPIC "homeassistant/chicken_brooder/logger"
#define MQTT_TARGET_TEMPERATURE_TOPIC "homeassistant/chicken_brooder/target_temperature/state"
#define MQTT_TARGET_TEMPERATURE_COMMAND_TOPIC "homeassistant/chicken_brooder/target_temperature/command"
#define MQTT_LIGHT_COMMAND_TOPIC "homeassistant/chicken_brooder/light/command"
#define MQTT_LIGHT_TOPIC "homeassistant/chicken_brooder/light/state"
#define MQTT_TEMPERATURE_TOPIC "homeassistant/chicken_brooder/temperature/state"
#define MQTT_HUMIDITY_TOPIC "homeassistant/chicken_brooder/humidity/state"

class MqttManager
{
public:
  MqttManager(const char *mqttServer, int mqttPort, const char *mqttUser, const char *mqttPassword, State<bool> *lightState, State<float> *targetTemperature, State<float> *temperature, State<float> *humidity);
  void begin();
  void loop();
  void publishTemperature(float temperature);
  void publishHumidity(float humidity);
  void publishLightState(bool state);
  void publishLogMessage(const char *message);
  bool isStarted() { return started; }

  PubSubClient mqttClient;

private:

  WiFiClient wifiClient;

  const char *m_mqttUser;
  const char *mqttPassword;
  const char *mqttServer;
  int mqttPort;
  bool started = false;
  State<bool> *lightState;
  State<float> *targetTemperature;
  State<float> *temperature;
  State<float> *humidity;
};

#endif // MQTT_MANAGER_H