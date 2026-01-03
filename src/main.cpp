#include <Arduino.h>
#include <Wire.h>
#include <esp_ota_ops.h> // For rollback functionality

#include "DisplayManager.h"
#include "InputManager.h"
#include "Lamp.h"
#include "NetworkManager.h"
#include "BrooderLog.h"
#include "OtaManager.h"
#include "MqttManager.h"
#include "PushButton.h"
#include "State.h"
#include "StateControlledLamp.h"
#include "PresistanceManager.h"
#include "Sensor.h"
#include "Thermostat.h"
#include "WiFi.h"
#include "secrets.h"
#include "version.h"


// Define the pins and thresholds
#define DHTPIN 27
#define DHTTYPE DHT11
#define LAMP1_PIN 32
#define LAMP2_PIN 33
#define RELAY_3_PIN 25
#define RELAY_4_PIN 26

// Button pins
#define BUTTON_INC_PIN 34
#define BUTTON_DEC_PIN 35

#define BUTTON_LIGHT_PIN 14

// OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

extern "C" bool verifyRollbackLater() {
    // Return true to delay rollback verification until esp_ota_mark_app_valid_cancel_rollback() is called
    return true;
}

// Create shared state for the target temperature
State<float> targetTemperature(26.0);

// Create shared state for temperature and humidity
State<float> temperature(25.0f);
State<float> humidity(60.0f);

// Create a State<bool> for the lamp
State<bool> lightState(false);

State<bool> heatingLamp1State(false);
State<bool> heatingLamp2State(false);
State<bool> relay4State(false);

// Create an array of relay states
State<bool> *relayStates[] = {&heatingLamp1State, &heatingLamp2State, &lightState, &relay4State};


// Create Sensor and Lamp objects
Sensor sensor(DHTPIN, DHTTYPE, &temperature, &humidity);
StateControlledLamp heatingLamp1(LAMP1_PIN, &heatingLamp1State);
StateControlledLamp heatingLamp2(LAMP2_PIN, &heatingLamp2State);
StateControlledLamp light(RELAY_3_PIN, &lightState);
PushButton lightButton(BUTTON_LIGHT_PIN, &lightState);


// Create the thermostat, display manager, and input manager objects
Thermostat thermostat(&temperature, heatingLamp1, heatingLamp2, &targetTemperature);
DisplayManager displayManager(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, SCREEN_ADDRESS, &temperature, &humidity, &targetTemperature, relayStates, 4);
InputManager inputManager(BUTTON_INC_PIN, BUTTON_DEC_PIN, &targetTemperature);

NetworkManager networkManager(WIFI_SSID, WIFI_PASSWORD);
OtaManager otaManager;
MqttManager mqttManager(MQTT_SERVER, MQTT_PORT, MQTT_USER, MQTT_PASSWORD, &lightState, &targetTemperature, &temperature, &humidity);

PresistanceManager persistenceManager("ChickenBrooder");

bool running_in_ap_mode = false;

bool healthCheck() {

  return true;
}

void rollbackToPreviousImage() {
  BROODER_LOG_E("Rolling back to the previous firmware image...");
  
  const esp_partition_t* runningPartition = esp_ota_get_running_partition();

  if (esp_ota_mark_app_invalid_rollback_and_reboot() == ESP_OK) {
    BROODER_LOG_E("Rollback successful. Rebooting...");
    delay(1000);
    ESP.restart();
  } else {
    BROODER_LOG_E("Rollback failed. Staying on the current firmware.");
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize the network manager
  networkManager.begin(WIFI_STA);

  int n_retries = 1;
  while (!networkManager.isStarted() && n_retries < 3){
    delay(5000);
    networkManager.begin();
    n_retries++;
  }

  if (!networkManager.isStarted()){
    running_in_ap_mode = true;
    if (!networkManager.begin(WIFI_AP)){
      BROODER_LOG_E("Failed to start network!");
      rollbackToPreviousImage();
    }
  }

  delay(1000);

  BrooderLog::begin(6666);
  BrooderLog::setUdpTarget(IPAddress(192, 168, 1, 60));


  BROODER_LOG_I("========================================");

  if (!running_in_ap_mode) {
    BROODER_LOG_I("Starting Chicken Brooder by Maia...");
  } else {
    BROODER_LOG_I("Starting Chicken Brooder by Maia in AP Mode...");
  }

  BROODER_LOG_I("Build Version: %u", BUILD_VERSION);
  BROODER_LOG_I("Build Date: %s", BUILD_DATE);
  BROODER_LOG_I("Build Time: %s", BUILD_TIME);
  BROODER_LOG_I("========================================");

  otaManager.begin();

  persistenceManager.begin();

  if (!running_in_ap_mode) {
    mqttManager.begin();
    BROODER_LOG_D("MQTT Manager Initialized");
  }

  // Initialize the sensor
  sensor.begin();
  BROODER_LOG_D("Sensor Initialized");

  // Initialize the lamps
  heatingLamp1.begin();
  heatingLamp2.begin();
  BROODER_LOG_D("Heating lamps Initialized");

  lightButton.begin();
  light.begin();
  BROODER_LOG_D("Light Initialized");

  // Initialize the thermostat
  thermostat.begin();
  BROODER_LOG_D("Thermostat Initialized");

  // Initialize the OLED display
  if (!displayManager.begin()) {
    BROODER_LOG_E("Display initialisation failed");
  }

  // Initialize input manager
  inputManager.begin();

  pinMode(RELAY_3_PIN, OUTPUT); // Set lamp pin as output
  digitalWrite(RELAY_3_PIN, LOW); // Ensure lamp is off at startup
  pinMode(RELAY_4_PIN, OUTPUT); // Set relay pin as output
  digitalWrite(RELAY_4_PIN, LOW); // Ensure relay is off at startup

  persistenceManager.manageState(&targetTemperature, "target");
  persistenceManager.manageState(&lightState, "light_state");

  targetTemperature.addListener([](float newValue) {
    BROODER_LOG_D("Target temperature updated: %.2f", newValue);
  });

  // Perform health check and validate flash image
  if (!healthCheck()) {
    BROODER_LOG_E("Health check failed. Rolling back to the previous image...");
    rollbackToPreviousImage();
  } else {
    esp_ota_mark_app_valid_cancel_rollback();
    BROODER_LOG_D("Health check passed. Flash image is valid.");
  }

}

void loop() {

  if (networkManager.isStarted()) {
    networkManager.run();
  } else {
    networkManager.begin();
  }

  if (otaManager.isStarted()){
    otaManager.run();
  } else {
    otaManager.begin();
  }

  BrooderLog::loop();

  if (!running_in_ap_mode){
    if (mqttManager.isStarted()) {
      mqttManager.loop();
    } else {
      mqttManager.begin();
    }
  }
  
  // Update the sensor readings
  sensor.update();


  // Update the thermostat (handles temperature reading and lamp control)
  thermostat.update();

  // Update the input manager (handles button presses)
  inputManager.update();
  
  lightButton.update();

  // Update the display manager
  displayManager.updateDisplay();

}