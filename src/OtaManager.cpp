#include "OtaManager.h"
#include <ArduinoOTA.h>
#include "BrooderLog.h"

bool OtaManager::begin(){
  ArduinoOTA.onStart([]() {
    BROODER_LOG_I("Start updating...");
  });
  
  ArduinoOTA.onEnd([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    BROODER_LOG_I("End updating %s", type);
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    BROODER_LOG_I("Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    BROODER_LOG_E("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      BROODER_LOG_E("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      BROODER_LOG_E("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      BROODER_LOG_E("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      BROODER_LOG_E("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      BROODER_LOG_E("End Failed");
    }
  });

  ArduinoOTA.setHostname("ChickenBrooder");
  ArduinoOTA.begin();
  BROODER_LOG_D("Successfully initialised OTA Manager.");

  started = true;

  return true;
}

void OtaManager::run(){
    ArduinoOTA.handle();
}