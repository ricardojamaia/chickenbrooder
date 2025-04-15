#include "DisplayManager.h"
#include "DebugBrooder.h" // Include the debug macro

DisplayManager::DisplayManager(uint8_t screenWidth, uint8_t screenHeight, TwoWire *wire, int8_t resetPin, uint8_t i2cAddress)
    : display(screenWidth, screenHeight, wire, resetPin), screenWidth(screenWidth), screenHeight(screenHeight), currentTemperature(NAN), currentHumidity(NAN) {}

bool DisplayManager::begin() {
  DEBUG_BROODER_PRINTLN("Initializing OLED display...");
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    DEBUG_BROODER_PRINTLN("OLED display initialization failed!");
    return false; // Initialization failed
  }
  display.clearDisplay();
  display.display();
  DEBUG_BROODER_PRINTLN("OLED display initialized successfully.");
  return true;
}

void DisplayManager::setTemperature(float temperature) {
  currentTemperature = temperature;
}

void DisplayManager::setHumidity(float humidity) {
  currentHumidity = humidity;
}

void DisplayManager::updateDisplay() {
  DEBUG_BROODER_PRINTLN("Updating OLED display...");
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Display "Temperature" label
  centerText("Temperature:", 1, 0);

  // Display temperature value in large font
  String tempStr = String(currentTemperature, 1) + " C";
  centerText(tempStr.c_str(), 2, 10);

  // Display "Humidity" label
  centerText("Humidity:", 1, 40);

  // Display humidity value in large font
  String humStr = String(currentHumidity, 1) + " %";
  centerText(humStr.c_str(), 2, 50);

  display.display();
}

void DisplayManager::centerText(const char *text, uint8_t textSize, int16_t y) {

  int16_t x1, y1;
  uint16_t w, h;

  display.setTextSize(textSize);
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  int16_t x = (screenWidth - w) / 2; // Center horizontally
  display.setCursor(x, y);
  display.println(text);
}