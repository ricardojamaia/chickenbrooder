#include "DebugBrooder.h"

// Define the UDP object
#ifdef UDP_SERIAL_MONITOR
WiFiUDP udp;
const char* debugHost = "192.168.1.14";
const int debugPort = 4444;
#endif