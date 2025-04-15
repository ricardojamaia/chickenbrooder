#include <Arduino.h>
#include <Wire.h>
#include "Thermostat.h"
#include "DebugBrooder.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "StateValue.h"

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
StateValue lowThreshold(32.0);
StateValue targetTemperature(38.0);

// Create shared state for temperature and humidity
StateValue temperature(25.0); // Initial temperature
StateValue humidity(60.0);    // Initial humidity

// Create the thermostat, display manager, and input manager objects
Thermostat thermostat(DHTPIN, DHTTYPE, LAMP1_PIN, LAMP2_PIN, &lowThreshold, &targetTemperature, &temperature, &humidity);
DisplayManager displayManager(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, SCREEN_ADDRESS, &temperature, &humidity, &targetTemperature);
InputManager inputManager(BUTTON_INC_PIN, BUTTON_DEC_PIN, &targetTemperature);

void setup() {
  Serial.begin(9600);
  DEBUG_BROODER_PRINTLN("Thermostat Initialized");

  // Initialize the OLED display
  if (!displayManager.begin()) {
    Serial.println("Display initialisation failed");
    for (;;); // Don't proceed, loop forever
  }

  // Initialize input manager
  inputManager.begin();
}

void loop() {
  // Update the thermostat (handles temperature reading and lamp control)
  thermostat.update();

  // Update the input manager (handles button presses)
  inputManager.update();

  // Update the display manager
  displayManager.updateDisplay();
}