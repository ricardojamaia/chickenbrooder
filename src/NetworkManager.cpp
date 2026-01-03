#include <WiFi.h>
#include <Arduino.h>

#include "NetworkManager.h"
#include "BrooderLog.h"

NetworkManager::NetworkManager(const char* ssid, const char* password) {
  this->ssid = ssid;
  this->password = password;
}

bool NetworkManager::begin(wifi_mode_t wifi_mode) {
  this->wifi_mode = wifi_mode;

  if (!WiFi.mode(wifi_mode)){
    BROODER_LOG_E("Failed to set WiFi mode!");
    started = false;
    return false;
  }
  
  if (WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE)){
    if (!WiFi.setHostname(BROODER_HOSTNAME)){
      BROODER_LOG_E("Failed to set hostname. Continuing ...");
    }
  } else {
    BROODER_LOG_E("Failed WiFi config. will not set the hostname. Continuing ...");
  }
  

  if (wifi_mode == WIFI_AP){
    if (!WiFi.softAP(BROODER_AP_SSID, BROODER_AP_PASSWORD, 1, 0, 1)){
      BROODER_LOG_E("Failed to start WiFi Access Point (AP)!");
      started = false;
      return false;
    }
  } else if (!connectWiFi()){
    started = false;
    BROODER_LOG_E("Failed to connect to WiFi network: %s", ssid);
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
    BROODER_LOG_W("Already connected to WiFi");
    return true;
  }
 
  if (lastAttempt + RETRY_DELAY > millis()){
    return false;
  }

  BROODER_LOG_D("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    BROODER_LOG_E("Connection Failed!");
    lastAttempt = millis();
    return false;
  }
  BROODER_LOG_D("IP address: %s", WiFi.localIP());

  lastAttempt = 0;
  connected = true;

  return true;
}



void NetworkManager::run() {

  if (!started){
    BROODER_LOG_E("NetworkManager not started");
    return;
  }

  if (wifi_mode == WIFI_AP){
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    if (lastAttempt + RETRY_DELAY > millis()){
      return;
    }
    BROODER_LOG_E("WiFi disconnected, trying to reconnect...");
    lastAttempt = millis();

    if (!WiFi.reconnect()) {
      BROODER_LOG_E("Failed to reconnect to WiFi");
      return;
    }
    BROODER_LOG_I("WiFi reconnected!");
  }

}
