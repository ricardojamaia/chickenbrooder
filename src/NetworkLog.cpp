#include "NetworkLog.h"
#include <esp_log.h>

// Static member initialization
WiFiUDP NetworkLog::udp;
IPAddress NetworkLog::targetIP;
uint16_t NetworkLog::udpPort = 6666;
bool NetworkLog::udpEnabled = false;
char NetworkLog::buffer[BUFFER_SIZE];
size_t NetworkLog::bufferPos = 0;
unsigned long NetworkLog::lastFlush = 0;

void NetworkLog::begin(uint16_t port) {
  udpPort = port;
  
  // Redirect ESP32 logging to our custom function
  esp_log_set_vprintf(vprintf_redirect);
  
  Serial.println("[NetworkLog] Initialized (Serial + UDP when WiFi connects)");
}

void NetworkLog::setUdpBroadcast() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[NetworkLog] WiFi not connected - UDP will start when connected");
    return;
  }
  
  // Set broadcast address
  targetIP = WiFi.localIP();
  targetIP[3] = 255;
  
  // Start UDP
  udp.begin(udpPort);
  udpEnabled = true;
  
  Serial.printf("[NetworkLog] UDP broadcast enabled -> %s:%d\n", 
                targetIP.toString().c_str(), udpPort);
}

void NetworkLog::setUdpTarget(IPAddress ip) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[NetworkLog] WiFi not connected - UDP will start when connected");
    targetIP = ip;
    return;
  }
  
  targetIP = ip;
  
  // Start UDP
  udp.begin(udpPort);
  udpEnabled = true;
  
  Serial.printf("[NetworkLog] UDP target set -> %s:%d\n", 
                targetIP.toString().c_str(), udpPort);
}

void NetworkLog::enableUdp(bool enable) {
  if (enable && !udpEnabled && WiFi.status() == WL_CONNECTED) {
    udp.begin(udpPort);
    udpEnabled = true;
    Serial.println("[NetworkLog] UDP enabled");
  } else if (!enable && udpEnabled) {
    flushBuffer();
    udp.stop();
    udpEnabled = false;
    Serial.println("[NetworkLog] UDP disabled");
  }
}

void NetworkLog::loop() {
  // Auto-enable UDP when WiFi connects
  if (!udpEnabled && WiFi.status() == WL_CONNECTED && targetIP[0] != 0) {
    udp.begin(udpPort);
    udpEnabled = true;
    Serial.println("[NetworkLog] UDP auto-enabled (WiFi connected)");
  }
  
  // Auto-disable UDP when WiFi disconnects
  if (udpEnabled && WiFi.status() != WL_CONNECTED) {
    udpEnabled = false;
    bufferPos = 0;  // Clear buffer
    Serial.println("[NetworkLog] UDP auto-disabled (WiFi disconnected)");
  }
  
  // Periodic flush
  if (udpEnabled && bufferPos > 0 && (millis() - lastFlush) >= FLUSH_INTERVAL) {
    flushBuffer();
  }
}

int NetworkLog::vprintf_redirect(const char *format, va_list args) {
  static char lineBuffer[256];
  
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

void NetworkLog::addToBuffer(const char* data, size_t len) {
  // If message is too large for buffer, flush and send immediately
  if (len >= BUFFER_SIZE) {
    flushBuffer();
    udp.beginPacket(targetIP, udpPort);
    udp.write((const uint8_t*)data, len);
    udp.endPacket();
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

void NetworkLog::flushBuffer() {
  if (!udpEnabled || bufferPos == 0) return;
  
  if (WiFi.status() == WL_CONNECTED) {
    udp.beginPacket(targetIP, udpPort);
    udp.write((const uint8_t*)buffer, bufferPos);
    udp.endPacket();
  }
  
  bufferPos = 0;
  lastFlush = millis();
}
