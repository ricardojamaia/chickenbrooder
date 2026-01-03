#ifndef NETWORK_LOG_H
#define NETWORK_LOG_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

class NetworkLog {
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

#endif // NETWORK_LOG_H
