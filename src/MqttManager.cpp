#include <MqttManager.h>
#include "DebugBrooder.h"

MqttManager::MqttManager(const char *mqttServer, int mqttPort, const char *mqttUser, const char *mqttPassword,
                         State<bool> *lightState, State<float> *targetTemperature, State<float> *temperature, State<float> *humidity)
    : mqttClient(wifiClient), m_mqttUser(mqttUser), mqttPassword(mqttPassword), mqttServer(mqttServer), mqttPort(mqttPort),
      lightState(lightState), targetTemperature(targetTemperature), temperature(temperature), humidity(humidity) {
  this->mqttClient.setServer(mqttServer, mqttPort);
}

void MqttManager::begin() {
  started = mqttClient.connect("ChickenBrooder", m_mqttUser, mqttPassword);
  if (!started) {
    DEBUG_BROODER_PRINTLN("MQTT connection failed");
    return;
  }
  
  DEBUG_BROODER_PRINTLN("MQTT connected");

  mqttClient.setCallback([this](char* topic, byte* payload, unsigned int length) {
    // Handle incoming MQTT messages here
    if (strcmp(topic, MQTT_LIGHT_COMMAND_TOPIC) == 0) {
      if (strncmp((char*)payload, "On", length) == 0) {
        DEBUG_BROODER_PRINTLN("Received remote command to turn light on");
        lightState->setValue(true);
        return;
      }
      if (strncmp((char*)payload, "Off", length) == 0) {
        DEBUG_BROODER_PRINTLN("Received remote command to turn light off");
        lightState->setValue(false);
        return;
      }
      DEBUG_BROODER_PRINTLN("Received unknown command");
      return;
    } else if (strcmp(topic, MQTT_TARGET_TEMPERATURE_COMMAND_TOPIC) == 0) {
      char buffer[10];
      strncpy(buffer, (char*)payload, length);
      buffer[length] = '\0'; // Null-terminate the string
      float newTargetTemperature = atof(buffer);
      DEBUG_BROODER_PRINT("Received remote command to set target temperature: ");
      DEBUG_BROODER_PRINTLN(newTargetTemperature);
      targetTemperature->setValue(newTargetTemperature);
    } else {
      DEBUG_BROODER_PRINT("Unexpected message on topic: ");
      DEBUG_BROODER_PRINTLN(topic);
    }
  });

  mqttClient.subscribe(MQTT_LIGHT_COMMAND_TOPIC);
  mqttClient.subscribe(MQTT_TARGET_TEMPERATURE_COMMAND_TOPIC);

  temperature->addListener([this](float newValue) {
    publishTemperature(newValue);
    DEBUG_BROODER_PRINT("Temperature update published: ");
    DEBUG_BROODER_PRINTLN(newValue);
  });
  humidity->addListener([this](float newValue) {
    publishHumidity(newValue);
    DEBUG_BROODER_PRINT("Humidity update published: ");
    DEBUG_BROODER_PRINTLN(newValue);
  });
  lightState->addListener([this](bool newValue) {
    publishLightState(newValue);
    DEBUG_BROODER_PRINT("Light state update published: ");
    DEBUG_BROODER_PRINTLN(newValue);
  });
  targetTemperature->addListener([this](float newValue) {
    char payload[50];
    snprintf(payload, sizeof(payload), "{\"target_temperature\": %.1f}", newValue);
    mqttClient.publish(MQTT_TARGET_TEMPERATURE_TOPIC, payload);
    DEBUG_BROODER_PRINT("Target temperature update published: ");
    DEBUG_BROODER_PRINTLN(newValue);
  });

  started = true;

}

void MqttManager::publishTemperature(float temperature) {
  if (mqttClient.connected()) {
    char payload[50];
    snprintf(payload, sizeof(payload), "{\"temperature\": %.1f}", temperature);
    mqttClient.publish(MQTT_TEMPERATURE_TOPIC, payload);
  }
}
void MqttManager::publishHumidity(float humidity) {
  if (mqttClient.connected()) {
    char payload[50];
    snprintf(payload, sizeof(payload), "{\"humidity\": %.1f}", humidity);
    mqttClient.publish(MQTT_HUMIDITY_TOPIC, payload);
  }
}
void MqttManager::publishLightState(bool state) {
  if (mqttClient.connected()) {
    char payload[50];
    snprintf(payload, sizeof(payload), "{\"light_state\": %s}", state ? "On" : "Off");
    mqttClient.publish(MQTT_LIGHT_TOPIC, payload);
  }
}

void::MqttManager::publishLogMessage(const char *message) {
  if (mqttClient.connected()) {
    mqttClient.publish(MQTT_LOGGER_TOPIC, message);
  }
}

void MqttManager::loop() {
  if (mqttClient.connected()) {
    mqttClient.loop();
  } else {
    DEBUG_BROODER_PRINTLN("MQTT client not connected, attempting to reconnect...");
    if (mqttClient.connect("ChickenBrooder", m_mqttUser, mqttPassword)) {
      DEBUG_BROODER_PRINTLN("MQTT reconnected");
      mqttClient.loop();
    } else {
      DEBUG_BROODER_PRINTLN("MQTT reconnection failed");
    }
  }
  
}

