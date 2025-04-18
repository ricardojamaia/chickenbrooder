#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include "State.h"

class DisplayManager {
public:
  DisplayManager(uint8_t screenWidth, uint8_t screenHeight, TwoWire *wire, int8_t resetPin, uint8_t i2cAddress, State<float> *temperature, State<float> *humidity, State<float> *targetTemperature);
  bool begin();
  void updateDisplay();
  void showTargetTemperature(float targetTemperature, unsigned long duration = 2000);
  void setUpdateRequired() { updateRequired = true; }

private:
  Adafruit_SSD1306 display;
  uint8_t screenWidth;
  uint8_t screenHeight;
  State<float> *temperature;
  State<float> *humidity;
  State<float> *targetTemperature;

  // Flag to track if an update is required
  bool updateRequired = false;

  // Temporary message state
  bool showingTemporaryMessage = false;
  unsigned long temporaryMessageStartTime = 0;
  unsigned long temporaryMessageDuration = 0;

  void centerText(const char *text, uint8_t textSize, int16_t y);
};

#endif