#ifndef DEBUG_BROODER_H
#define DEBUG_BROODER_H

#include <Arduino.h>

// Check if DEBUG_BROODER is defined in the build flags
#ifdef DEBUG_BROODER
#define DEBUG_BROODER_PRINT(...) { Serial.print(__VA_ARGS__); }
#define DEBUG_BROODER_PRINTLN(...) { Serial.println(__VA_ARGS__); }
#else
#define DEBUG_BROODER_PRINT(...) {} // Placeholder if DEBUG_BROODER is not defined
#define DEBUG_BROODER_PRINTLN(...) {} // Placeholder if DEBUG_BROODER is not defined
#endif

#endif