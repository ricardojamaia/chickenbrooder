#ifndef DEBUG_BROODER_H
#define DEBUG_BROODER_H

#include <esp_log.h>

// Map old DEBUG_BROODER macros to ESP32 logging
// These will now go through NetworkLog's vprintf redirect
#define DEBUG_BROODER_PRINT(...)    do{ } while(0)
#define DEBUG_BROODER_PRINTLN(...)  do{ } while(0)
#define DEBUG_BROODER_PRINTF(...)   do{ } while(0)

#endif // DEBUG_BROODER_H
