#include <Arduino.h>
#include <Wire.h>
#include "Thermostat.h"
#include "DebugBrooder.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "ThresholdValue.h"

// Define the pins and thresholds
#define DHTPIN 2
#define DHTTYPE DHT11
#define LAMP1_PIN 5
#define LAMP2_PIN 6

// Button pins
#define BUTTON_INC_PIN 3
#define BUTTON_DEC_PIN 4

// OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// Create shared state for the thresholds
ThresholdValue lowThreshold(32.0);
ThresholdValue highThreshold(38.0);

// Create the thermostat, display manager, and input manager objects
Thermostat thermostat(DHTPIN, DHTTYPE, LAMP1_PIN, LAMP2_PIN, &lowThreshold, &highThreshold);
DisplayManager displayManager(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, SCREEN_ADDRESS);
InputManager inputManager(BUTTON_INC_PIN, BUTTON_DEC_PIN, &highThreshold);


void setup() {
  Serial.begin(9600);
  DEBUG_BROODER_PRINTLN("Thermostat Initialized");

  // Initialize the OLED display
  if (!displayManager.begin()) {
    Serial.println("Display initialisation failed");
    for (;;); // Don't proceed, loop forever
  }

  // Register the listener for the high threshold
  highThreshold.addListener([](float newValue) {
    displayManager.showTargetTemperature(newValue);
  });

  // Initialize input manager
  inputManager.begin();
}

void loop() {
  // Update the thermostat (handles temperature reading and lamp control)
  thermostat.update();

  // Update the input manager (handles button presses)
  inputManager.update();

  // Get the current temperature, humidity, and high threshold
  float temperature = thermostat.getTemperature();
  float humidity = thermostat.getHumidity();

  // Static variables to store previous temperature and humidity
  static float previousTemperature = NAN;
  static float previousHumidity = NAN;

  // Check if temperature or humidity has changed
  if (temperature != previousTemperature || humidity != previousHumidity) {
    DEBUG_BROODER_PRINTLN("Temperature or humidity changed. Updating display...");

    // Update the display manager with the new values
    displayManager.setTemperature(temperature);
    displayManager.setHumidity(humidity);
    displayManager.updateDisplay();

    // Update the previous values
    previousTemperature = temperature;
    previousHumidity = humidity;
  }

 
}