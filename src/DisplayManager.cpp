#include "DisplayManager.h"
#include "DebugBrooder.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

DisplayManager::DisplayManager(uint8_t screenWidth, uint8_t screenHeight, TwoWire *wire, int8_t resetPin, uint8_t i2cAddress,
                               State<float> *temperature, State<float> *humidity, State<float> *targetTemperature, State<bool> *relayStates[], uint8_t relayCount)
    : display(screenWidth, screenHeight, wire, resetPin), screenWidth(screenWidth), screenHeight(screenHeight),
      temperature(temperature), humidity(humidity), targetTemperature(targetTemperature), relayStates(relayStates), relayCount(relayCount) {

  // Register listeners for temperature, humidity, and target temperature
  temperature->addListener([this](float newValue) {
    updateRequired = true;
  });

  humidity->addListener([this](float newValue) {
    updateRequired = true;
  });

  targetTemperature->addListener([this](float newValue) {
    showTargetTemperature(newValue);
  });

  // Register listeners for relay states
  for (uint8_t i = 0; i < relayCount; i++) {
    relayStates[i]->addListener([this](bool newValue) {
      updateRequired = true;
    });
  }
}

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

void DisplayManager::updateDisplay() {
  // Check if a temporary message is being displayed
  if (showingTemporaryMessage) {
    unsigned long currentTime = millis();
    if (currentTime - temporaryMessageStartTime < temporaryMessageDuration) {
      return;
    } else {
      // Temporary message duration has elapsed, return to normal display
      showingTemporaryMessage = false;
      updateRequired = true; // Set update required to refresh the display
    }
  }

  // Only update the display if an update is required
  if (updateRequired) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // Display "Temperature" label
    centerText("Temperature:", 1, 10);

    // Display temperature value in large font
    String tempStr = String(temperature->getValue(), 1) + " C";
    centerText(tempStr.c_str(), 2, 20);

    // Display "Humidity" label
    centerText("Humidity:", 1, 40);

    // Display humidity value in large font
    String humStr = String(humidity->getValue(), 1) + " %";
    centerText(humStr.c_str(), 2, 50);

    drawStatusBar();

    display.display();

    // Reset the flag
    updateRequired = false;
  }
}

void DisplayManager::showTargetTemperature(float targetTemperature, unsigned long duration) {
  DEBUG_BROODER_PRINT("Displaying target temperature: ");
  DEBUG_BROODER_PRINTLN(targetTemperature);

  temporaryMessageDuration = duration;
  temporaryMessageStartTime = millis();
  showingTemporaryMessage = true;

  display.clearDisplay();

  // Format the target temperature as a string
  String tempStr = String(targetTemperature, 1) + " C";
  centerText(tempStr.c_str(), 2, 30);

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

void DisplayManager::drawStatusBar() {
  int padding = 8; // Padding around the status bar
  int spacing = 4;  // Spacing between icons
  int iconWidth = ((screenWidth - padding * 2) / relayCount) - spacing; // Size of each icon
  int iconHeight = 4;

  int x = padding;      // Initial x position
  int y = 0;            // Y position for the status bar

  for (uint8_t i = 0; i < relayCount; i++) {
    if (relayStates[i]->getValue()) {
      display.fillRect(x, y, iconWidth, iconHeight, SSD1306_WHITE); // Filled rectangle for ON
    } else {
      display.drawRect(x, y, iconWidth, iconHeight, SSD1306_WHITE); // Outline rectangle for OFF
    }
    x += iconWidth + spacing;
  }
}
