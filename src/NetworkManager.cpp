#include <WiFi.h>

#include <Arduino.h>

#include "NetworkManager.h"
#include "DebugBrooder.h"

NetworkManager::NetworkManager(const char* ssid, const char* password) {
  this->ssid = ssid;
  this->password = password;
}

bool NetworkManager::begin(wifi_mode_t wifi_mode) {
  this->wifi_mode = wifi_mode;

  if (!WiFi.mode(wifi_mode)){
    DEBUG_BROODER_PRINTLN("Failed to set WiFi mode!");
    started = false;
    return false;
  }
  
  if (WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE)){
    if (!WiFi.setHostname(BROODER_HOSTNAME)){
      DEBUG_BROODER_PRINTLN("Failed to set hostname. Continuing ...");
    }
  } else {
    DEBUG_BROODER_PRINTLN("Failed WiFi config. will not set the hostname. Continuing ...");
  }
  

  if (wifi_mode == WIFI_AP){
    if (!WiFi.softAP(BROODER_AP_SSID, BROODER_AP_PASSWORD, 1, 0, 1)){
      DEBUG_BROODER_PRINTLN("Failed to start WiFi Access Point (AP)!");
      started = false;
      return false;
    }
  } else if (!connectWiFi()){
    started = false;
    DEBUG_BROODER_PRINT("Failed to connect to WiFi network: ");
    DEBUG_BROODER_PRINTLN(ssid);
    return false;
  }

  started = true;
  return true;

}

bool NetworkManager::connectWiFi() {
  if (wifi_mode == WIFI_AP){
    return true;
  }

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

  if (wifi_mode == WIFI_AP){
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

}
