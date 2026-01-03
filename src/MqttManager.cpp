#include <MqttManager.h>
#include "BrooderLog.h"

#define CONNECT_ATTEMPTS_DELAY_MILLIS 3 * 60 * 1000

MqttManager::MqttManager(const char *mqttServer, int mqttPort, const char *mqttUser, const char *mqttPassword,
                         State<bool> *lightState, State<float> *targetTemperature, State<float> *temperature, State<float> *humidity)
    : mqttClient(wifiClient), m_mqttUser(mqttUser), mqttPassword(mqttPassword), mqttServer(mqttServer), mqttPort(mqttPort),
      lightState(lightState), targetTemperature(targetTemperature), temperature(temperature), humidity(humidity) {
  this->mqttClient.setServer(mqttServer, mqttPort);
}

void MqttManager::begin() {
  long actual_millis = millis();

  if ((m_lastConnectAttempt != 0) && (m_lastConnectAttempt + CONNECT_ATTEMPTS_DELAY_MILLIS > actual_millis)){
    return;
  }

  m_lastConnectAttempt = actual_millis;

  started = mqttClient.connect("ChickenBrooder", m_mqttUser, mqttPassword);
  if (!started) {
    BROODER_LOG_E("MQTT connection failed");
    return;
  }
  
  BROODER_LOG_D("MQTT connected");

  mqttClient.setCallback([this](char* topic, byte* payload, unsigned int length) {
    // Handle incoming MQTT messages here
    if (strcmp(topic, MQTT_LIGHT_COMMAND_TOPIC) == 0) {
      if (strncmp((char*)payload, "On", length) == 0) {
        BROODER_LOG_D("Received remote command to turn light on");
        lightState->setValue(true);
        return;
      }
      if (strncmp((char*)payload, "Off", length) == 0) {
        BROODER_LOG_D("Received remote command to turn light off");
        lightState->setValue(false);
        return;
      }
      BROODER_LOG_W("Received unknown command");
      return;
    } else if (strcmp(topic, MQTT_TARGET_TEMPERATURE_COMMAND_TOPIC) == 0) {
      char buffer[10];
      strncpy(buffer, (char*)payload, length);
      buffer[length] = '\0'; // Null-terminate the string
      float newTargetTemperature = atof(buffer);
      BROODER_LOG_D("Received remote command to set target temperature: %.2f", newTargetTemperature);
      targetTemperature->setValue(newTargetTemperature);
    } else {
      BROODER_LOG_W("Unexpected message on topic: %s", topic);
    }
  });

  mqttClient.subscribe(MQTT_TARGET_TEMPERATURE_COMMAND_TOPIC);
  mqttClient.subscribe(MQTT_LIGHT_COMMAND_TOPIC);

  temperature->addListener([this](float newValue) {
    publishTemperature(newValue);
  });
  humidity->addListener([this](float newValue) {
    publishHumidity(newValue);
  });
  lightState->addListener([this](bool newValue) {
    publishLightState(newValue);
  });
  targetTemperature->addListener([this](float newValue) {
    char payload[50];
    snprintf(payload, sizeof(payload), "{\"target_temperature\": %.1f}", newValue);
    mqttClient.publish(MQTT_TARGET_TEMPERATURE_TOPIC, payload);
  });

  started = true;

  BROODER_LOG_D("MQTT successfully initialised.");

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
    snprintf(payload, sizeof(payload), "{\"light_state\": \"%s\"}", state ? "On" : "Off");
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
    long actual_millis = millis();
    
    if (m_lastConnectAttempt + CONNECT_ATTEMPTS_DELAY_MILLIS > actual_millis){
      return;
    }
    m_lastConnectAttempt = actual_millis;

    BROODER_LOG_D("MQTT client not connected, attempting to reconnect...");
    if (mqttClient.connect("ChickenBrooder", m_mqttUser, mqttPassword)) {
      BROODER_LOG_D("MQTT reconnected");
      mqttClient.loop();
    } else {
      BROODER_LOG_E("MQTT reconnection failed");
    }
  }
  
}

