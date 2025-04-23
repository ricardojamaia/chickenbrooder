#include <WiFi.h>
#include <ArduinoOTA.h>

#include <Arduino.h>

#include "NetworkManager.h"
#include "DebugBrooder.h"

NetworkManager::NetworkManager(const char* ssid, const char* password) {
  this->ssid = ssid;
  this->password = password;
}

bool NetworkManager::begin() {

  WiFi.mode(WIFI_STA);
  
  if (!connectWiFi()){
    started = false;
    return false;
  }

  DEBUG_BROODER_PRINTLN("Starting OTA");
  ArduinoOTA.setHostname("Chicken Brooder");

  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_SPIFFS
      type = "filesystem";
    }
    DEBUG_BROODER_PRINTLN("Start updating " + type);
  })
  .onEnd([]() {
    DEBUG_BROODER_PRINTLN("\nEnd");
    ESP.restart();
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    DEBUG_BROODER_PRINTF("Progress: %u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      DEBUG_BROODER_PRINTLN("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      DEBUG_BROODER_PRINTLN("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      DEBUG_BROODER_PRINTLN("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      DEBUG_BROODER_PRINTLN("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      DEBUG_BROODER_PRINTLN("End Failed");
    }
  });

  ArduinoOTA.begin();

  started = true;
  return true;

}

bool NetworkManager::connectWiFi() {

  if (WiFi.status() == WL_CONNECTED){
    DEBUG_BROODER_PRINTLN("Already connected to WiFi");
    return true;
  }
 
  if (lastAttempt + RETRY_DELAY > millis()){
    return false;
  }

  DEBUG_BROODER_PRINTLN("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    DEBUG_BROODER_PRINTLN("Connection Failed!");
    lastAttempt = millis();
    return false;
  }
  DEBUG_BROODER_PRINTLN("IP address: ");
  DEBUG_BROODER_PRINTLN(WiFi.localIP());


  lastAttempt = 0;
  connected = true;

  return true;
}



void NetworkManager::run() {

  if (!started){
    DEBUG_BROODER_PRINTLN("NetworkManager not started");
    return;
  }
  if (WiFi.status() != WL_CONNECTED) {
    if (lastAttempt + RETRY_DELAY > millis()){
      return;
    }
    DEBUG_BROODER_PRINTLN("WiFi disconnected, trying to reconnect...");
    lastAttempt = millis();

    if (!WiFi.reconnect()) {
      DEBUG_BROODER_PRINTLN("Failed to reconnect to WiFi");
      return;
    }
    DEBUG_BROODER_PRINTLN("WiFi reconnected!");
  }

  ArduinoOTA.handle();
}
