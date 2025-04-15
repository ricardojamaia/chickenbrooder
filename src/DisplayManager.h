#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Adafruit_SSD1306.h>
#include <Arduino.h>

class DisplayManager {
public:
  DisplayManager(uint8_t screenWidth, uint8_t screenHeight, TwoWire *wire, int8_t resetPin, uint8_t i2cAddress);
  bool begin();
  void setTemperature(float temperature);
  void setHumidity(float humidity);
  void updateDisplay();
  void showTargetTemperature(float targetTemperature, unsigned long duration = 2000);

private:
  Adafruit_SSD1306 display;
  uint8_t screenWidth;
  uint8_t screenHeight;
  float currentTemperature;
  float currentHumidity;

  // Temporary message state
  bool showingTemporaryMessage = false;
  unsigned long temporaryMessageStartTime = 0;
  unsigned long temporaryMessageDuration = 0;
  String temporaryMessage;

  void centerText(const char *text, uint8_t textSize, int16_t y);
};

#endif