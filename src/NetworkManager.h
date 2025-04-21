
#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#define MAX_RETRIES 5
#define RETRY_DELAY 10000

class NetworkManager {
  public:
    NetworkManager(const char* ssid, const char* password);
    bool begin();
    bool connectWiFi();
    void run();
    bool isStarted() { return started; }

  private:
    long lastAttempt = -RETRY_DELAY;
    const char* ssid;
    const char* password;
    bool connected = false;
    bool started = false;
};

#endif // NETWORKMANAGER_H
