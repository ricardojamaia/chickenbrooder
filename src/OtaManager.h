#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

class OtaManager {
  public:
    bool begin();
    bool isStarted() { return started; }
    void run();
  private:
    bool started = false;
};

#endif
