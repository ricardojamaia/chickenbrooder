#ifndef BROODER_LOG_H
#define BROODER_LOG_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#define BROODER_LOG_LEVEL_NONE    ESP_LOG_NONE
#define BROODER_LOG_LEVEL_ERROR   ESP_LOG_ERROR
#define BROODER_LOG_LEVEL_WARN    ESP_LOG_WARN
#define BROODER_LOG_LEVEL_INFO    ESP_LOG_INFO
#define BROODER_LOG_LEVEL_DEBUG   ESP_LOG_DEBUG
#define BROODER_LOG_LEVEL_VERBOSE ESP_LOG_VERBOSE


#ifndef BROODER_LOG_LEVEL
#define BROODER_LOG_LEVEL BROODER_LOG_LEVEL_INFO
#endif

#define BROODER_LOG_E log_e
#define BROODER_LOG_W log_w
#define BROODER_LOG_I log_i
#define BROODER_LOG_D log_d
#define BROODER_LOG_V log_v

class BrooderLog {
public:
  static void begin(uint16_t udpPort = 6666);
  static void enableUdp(bool enable);
  static void setUdpBroadcast();
  static void setUdpTarget(IPAddress ip);
  static void loop();
  
  // Internal vprintf function (called by ESP32 logging system)
  static int vprintf_redirect(const char *format, va_list args);

private:
  static WiFiUDP udp;
  static IPAddress targetIP;
  static uint16_t udpPort;
  static bool udpEnabled;
  
  // Buffering for reliability
  static const size_t BUFFER_SIZE = 1024;
  static char buffer[BUFFER_SIZE];
  static size_t bufferPos;
  static unsigned long lastFlush;
  static const unsigned long FLUSH_INTERVAL = 100; // ms
  
  static void flushBuffer();
  static void addToBuffer(const char* data, size_t len);
};

#endif // BROODER_LOG_H
