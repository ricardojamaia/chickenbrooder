#include "OtaManager.h"
#include <ArduinoOTA.h>
#include "DebugBrooder.h"

bool OtaManager::begin(){
  ArduinoOTA.onStart([]() {
    Serial.println("Start updating...");
  });
  
  ArduinoOTA.onEnd([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    Serial.println("End updating " + type);
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });

  DEBUG_BROODER_PRINT("Starting OTA ...");
  ArduinoOTA.setHostname("ChickenBrooder");
  ArduinoOTA.begin();
  DEBUG_BROODER_PRINTLN(" done.");

  started = true;

  return true;
}

void OtaManager::run(){
    ArduinoOTA.handle();
}