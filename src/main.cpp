#include <Arduino.h>
#include <Wire.h>
#include "DebugBrooder.h"
#include "DisplayManager.h"
#include "InputManager.h"
#include "Lamp.h"
#include "PushButton.h"
#include "State.h"
#include "StateControlledLamp.h"
#include "Sensor.h"
#include "Thermostat.h"

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

#define BUTTON_LIGHT_PIN 14

// OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// Create shared state for the target temperature
State<float> targetTemperature(38.0);

// Create shared state for temperature and humidity
State<float> temperature(25.0f);
State<float> humidity(60.0f);

// Create a State<bool> for the lamp
State<bool> lightState(false);

// Create Sensor and Lamp objects
Sensor sensor(DHTPIN, DHTTYPE, &temperature, &humidity);
Lamp heatingLamp1(LAMP1_PIN);
Lamp heatingLamp2(LAMP2_PIN);
StateControlledLamp light(RELAY_3_PIN, &lightState);
PushButton lightButton(BUTTON_LIGHT_PIN, &lightState);


// Create the thermostat, display manager, and input manager objects
Thermostat thermostat(&temperature, heatingLamp1, heatingLamp2, &targetTemperature);
DisplayManager displayManager(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, SCREEN_ADDRESS, &temperature, &humidity, &targetTemperature);
InputManager inputManager(BUTTON_INC_PIN, BUTTON_DEC_PIN, &targetTemperature);

void setup() {
  Serial.begin(115200);

  DEBUG_BROODER_PRINTLN("Starting Brooder...");

  // Initialize the sensor
  sensor.begin();
  DEBUG_BROODER_PRINTLN("Sensor Initialized");

  // Initialize the lamps
  heatingLamp1.begin();
  heatingLamp2.begin();
  DEBUG_BROODER_PRINTLN("Heating lamps Initialized");

  lightButton.begin();
  light.begin();
  DEBUG_BROODER_PRINTLN("Light Initialized");

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
  
  // Update the sensor readings
  sensor.update();


  // Update the thermostat (handles temperature reading and lamp control)
  thermostat.update();

  // Update the input manager (handles button presses)
  inputManager.update();
  
  lightButton.update();

  // Update the display manager
  displayManager.updateDisplay();

  // Example: Increase temperature
  temperature.increase(0.5f);

  // Example: Decrease humidity
  humidity.decrease(1.0f);

  delay(50); // Small delay to debounce the button
}