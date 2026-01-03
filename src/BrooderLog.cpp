#include "BrooderLog.h"
#include <esp_log.h>

// Static member initialization
WiFiUDP BrooderLog::udp;
IPAddress BrooderLog::targetIP;
uint16_t BrooderLog::udpPort = 6666;
bool BrooderLog::udpEnabled = false;
char BrooderLog::buffer[BUFFER_SIZE];
size_t BrooderLog::bufferPos = 0;
unsigned long BrooderLog::lastFlush = 0;

void BrooderLog::begin(uint16_t port) {
  udpPort = port;

  esp_log_level_set("*", BROODER_LOG_LEVEL);
  
  // Redirect ESP32 logging to our custom function
  esp_log_set_vprintf(vprintf_redirect);
  
  BROODER_LOG_D("Initialized (Serial + UDP when WiFi connects)");
}

void BrooderLog::setUdpBroadcast() {
  if (WiFi.status() != WL_CONNECTED) {
    BROODER_LOG_D("WiFi not connected - UDP will start when connected");
    return;
  }
  
  // Set broadcast address
  targetIP = WiFi.localIP();
  targetIP[3] = 255;
  
  // Start UDP
  udp.begin(udpPort);
  udpEnabled = true;
  
  BROODER_LOG_I("UDP broadcast enabled -> %s:%d\n", 
                targetIP.toString().c_str(), udpPort);
}

void BrooderLog::setUdpTarget(IPAddress ip) {
  if (WiFi.status() != WL_CONNECTED) {
    BROODER_LOG_I("WiFi not connected - UDP will start when connected");
    targetIP = ip;
    return;
  }
  
  targetIP = ip;
  
  // Start UDP
  udp.begin(udpPort);
  udpEnabled = true;
  
  BROODER_LOG_D("UDP target set -> %s:%d\n", 
                targetIP.toString().c_str(), udpPort);
}

void BrooderLog::enableUdp(bool enable) {
  if (enable && !udpEnabled && WiFi.status() == WL_CONNECTED) {
    udp.begin(udpPort);
    udpEnabled = true;
    BROODER_LOG_D("UDP enabled");
  } else if (!enable && udpEnabled) {
    flushBuffer();
    udp.stop();
    udpEnabled = false;
    BROODER_LOG_D("UDP disabled");
  }
}

void BrooderLog::loop() {
  static unsigned long lastDebug = 0;
  static int loopCount = 0;
  
  loopCount++;
  
  // Auto-enable UDP when WiFi connects
  if (!udpEnabled && WiFi.status() == WL_CONNECTED && targetIP[0] != 0) {
    udp.begin(udpPort);
    udpEnabled = true;
    BROODER_LOG_D("UDP auto-enabled (WiFi connected)");
  }
  
  // Auto-disable UDP when WiFi disconnects
  if (udpEnabled && WiFi.status() != WL_CONNECTED) {
    udpEnabled = false;
    bufferPos = 0;
    BROODER_LOG_D("UDP auto-disabled (WiFi disconnected)");
  }
  
  // Periodic flush
  if (udpEnabled && bufferPos > 0 && (millis() - lastFlush) >= FLUSH_INTERVAL) {
    flushBuffer();
  }
}

int BrooderLog::vprintf_redirect(const char *format, va_list args) {
  static char lineBuffer[256];
  static unsigned long msgCount = 0;
  
  msgCount++;
  
  // Format the message
  int len = vsnprintf(lineBuffer, sizeof(lineBuffer), format, args);
  
  // Always send to Serial
  Serial.write((uint8_t*)lineBuffer, len);

  
  // Send to UDP if enabled
  if (udpEnabled && WiFi.status() == WL_CONNECTED) {
    addToBuffer(lineBuffer, len);
  }
  
  return len;
}

void BrooderLog::addToBuffer(const char* data, size_t len) {
  
  // If message is too large for buffer, flush and send immediately
  if (len >= BUFFER_SIZE) {
    flushBuffer();
    udp.beginPacket(targetIP, udpPort);
    udp.write((const uint8_t*)data, len);
    return;
  }
  
  // If adding would overflow, flush first
  if (bufferPos + len >= BUFFER_SIZE) {
    flushBuffer();
  }
  
  // Add to buffer
  memcpy(buffer + bufferPos, data, len);
  bufferPos += len;
  
  
  // Auto-flush if buffer is getting full (75%)
  if (bufferPos >= (BUFFER_SIZE * 3 / 4)) {
    flushBuffer();
  }
}

void BrooderLog::flushBuffer() {
  if (!udpEnabled || bufferPos == 0) {
    return;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    int result = udp.beginPacket(targetIP, udpPort);
    if (!result) {
      BROODER_LOG_E("beginPacket failed!");
    } else {
      size_t written = udp.write((const uint8_t*)buffer, bufferPos);
      
      result = udp.endPacket();
      if (!result) {
        BROODER_LOG_E("endPacket failed!");
      }
    }
  }

  bufferPos = 0;
  lastFlush = millis();
}