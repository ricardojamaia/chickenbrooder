#include <Arduino.h>
#include <Wire.h>
#include "Thermostat.h"
#include "DebugBrooder.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "StateValue.h"
#include "Lamp.h"

// Define the pins and thresholds
#define DHTPIN 27
#define DHTTYPE DHT11
#define LAMP1_PIN 32
#define LAMP2_PIN 33
#define RELAY_3_PIN 25
#define RELAY_4_PIN 26

// Button pins
#define BUTTON_INC_PIN 34
#define BUTTON_DEC_PIN 35

// OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// Create shared state for the thresholds
StateValue targetTemperature(38.0);

// Create shared state for temperature and humidity
StateValue temperature(25.0); // Initial temperature
StateValue humidity(60.0);    // Initial humidity

// Create Lamp objects
Lamp lamp1(LAMP1_PIN);
Lamp lamp2(LAMP2_PIN);

// Create the thermostat, display manager, and input manager objects
Thermostat thermostat(DHTPIN, DHTTYPE, lamp1, lamp2, &targetTemperature, &temperature, &humidity);
DisplayManager displayManager(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, SCREEN_ADDRESS, &temperature, &humidity, &targetTemperature);
InputManager inputManager(BUTTON_INC_PIN, BUTTON_DEC_PIN, &targetTemperature);

void setup() {
  Serial.begin(115200);

  DEBUG_BROODER_PRINTLN("Starting Brooder...");

  // Initialize the thermostat
  thermostat.begin();
  DEBUG_BROODER_PRINTLN("Thermostat Initialized");

  // Initialize the OLED display
  if (!displayManager.begin()) {
    Serial.println("Display initialisation failed");
    for (;;); // Don't proceed, loop forever
  }

  // Initialize input manager
  inputManager.begin();

  pinMode(RELAY_3_PIN, OUTPUT); // Set lamp pin as output
  digitalWrite(RELAY_3_PIN, LOW); // Ensure lamp is off at startup
  pinMode(RELAY_4_PIN, OUTPUT); // Set relay pin as output
  digitalWrite(RELAY_4_PIN, LOW); // Ensure relay is off at startup

  DEBUG_BROODER_PRINTLN("Brooder started");

}

void loop() {
  // Update the thermostat (handles temperature reading and lamp control)
  thermostat.update();

  // Update the input manager (handles button presses)
  inputManager.update();

  // Update the display manager
  displayManager.updateDisplay();
  }