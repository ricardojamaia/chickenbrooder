#ifndef DEBUG_BROODER_H
#define DEBUG_BROODER_H

#include <Arduino.h>

// Check if DEBUG_BROODER is defined in the build flags
#ifdef DEBUG_BROODER

#ifdef UDP_SERIAL_MONITOR
#include <WiFi.h>
#include <WiFiUdp.h>

// Define a UDP object for remote debugging
extern WiFiUDP udp;
extern const char* debugHost;
extern const int debugPort;

// Initialize remote debugging
inline void initDebug() {
    udp.begin(debugPort);
}

// Send debug messages over UDP
inline void remoteDebugPrint(const char* message) {
    if (WiFi.status() == WL_CONNECTED) {
        udp.beginPacket(debugHost, debugPort);
        udp.print(message);
        udp.endPacket();
    }
}
#define DEBUG_BROODER_PRINT(x) \
    do { \
        char buffer[256]; \
        snprintf(buffer, sizeof(buffer), "%s", String(x).c_str()); \
        remoteDebugPrint(buffer); \
    } while (0)
#define DEBUG_BROODER_PRINTLN(x) \
    do { \
        char buffer[256]; \
        snprintf(buffer, sizeof(buffer), "%s\n", String(x).c_str()); \
        remoteDebugPrint(buffer); \
    } while (0)
#define DEBUG_BROODER_PRINTF(fmt, ...) \
    do { \
        char buffer[256]; \
        snprintf(buffer, sizeof(buffer), fmt, __VA_ARGS__); \
        remoteDebugPrint(buffer); \
    } while (0)

#else
#define DEBUG_BROODER_PRINT(...) { Serial.print(__VA_ARGS__); }
#define DEBUG_BROODER_PRINTLN(...) { Serial.println(__VA_ARGS__); Serial.flush(); }
#define DEBUG_BROODER_PRINTF(...) { Serial.printf(__VA_ARGS__); Serial.flush(); }
#endif
#else
#define DEBUG_BROODER_PRINT(...) {} // Placeholder if DEBUG_BROODER is not defined
#define DEBUG_BROODER_PRINTLN(...) {} // Placeholder if DEBUG_BROODER is not defined
#define DEBUG_BROODER_PRINTF(...) {} // Placeholder if DEBUG_BROODER is not defined
#endif

#endif