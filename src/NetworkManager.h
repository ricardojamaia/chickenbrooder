
#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <WiFi.h>

#define MAX_RETRIES 5
#define RETRY_DELAY 10000

#ifndef BROODER_HOSTNAME
#define BROODER_HOSTNAME "BrooderByMaia"
#endif
#ifndef BROODER_AP_SSID
#define BROODER_AP_SSID "BrooderByMaia"
#endif

#ifndef BROODER_AP_PASSWORD
#define BROODER_AP_PASSWORD NULL
#endif

class NetworkManager {
  public:
    NetworkManager(const char* ssid, const char* password);
    bool begin(wifi_mode_t wifi_mode = WIFI_STA);
    bool connectWiFi();
    void run();
    bool isStarted() { return started; }

  private:
    wifi_mode_t wifi_mode = WIFI_STA;
    long lastAttempt = -RETRY_DELAY;
    const char* ssid;
    const char* password;
    bool connected = false;
    bool started = false;

};

#endif // NETWORKMANAGER_H
