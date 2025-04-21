#include <unity.h>
#include "../src/Thermostat.h"
#include "../src/Lamp.h"
#include "../src/State.h"

// Mock Lamp class to simulate lamp behavior
class MockLamp : public Lamp {
public:
    MockLamp(uint8_t pin) : Lamp(pin), turnedOn(false) {}

    void turnOn() override {
        turnedOn = true;
    }

    void turnOff() override {
        turnedOn = false;
    }

    bool getStatus() const override {
        return turnedOn;
    }

private:
    bool turnedOn;
};

// Test: Thermostat turns off both lamps when temperature exceeds target
void test_thermostat_turns_off_lamps() {
    State<float> temperature(40.0f); // Current temperature
    State<float> targetTemperature(38.0f); // Target temperature
    MockLamp lamp1(1);
    MockLamp lamp2(2);

    Thermostat thermostat(&temperature, lamp1, lamp2, &targetTemperature);

    thermostat.update();

    TEST_ASSERT_FALSE(lamp1.getStatus());
    TEST_ASSERT_FALSE(lamp2.getStatus());
}

// Test: Thermostat turns on both lamps when temperature is below low threshold
void test_thermostat_turns_on_lamps() {
    State<float> temperature(35.0f); // Current temperature
    State<float> targetTemperature(38.0f); // Target temperature
    MockLamp lamp1(1);
    MockLamp lamp2(2);

    Thermostat thermostat(&temperature, lamp1, lamp2, &targetTemperature);

    thermostat.update();

    TEST_ASSERT_TRUE(lamp1.getStatus());
    TEST_ASSERT_TRUE(lamp2.getStatus());
}

// Test: Thermostat alternates lamps when temperature is within range
void test_thermostat_alternates_lamps() {
    State<float> temperature(37.0f); // Current temperature
    State<float> targetTemperature(38.0f); // Target temperature
    MockLamp lamp1(1);
    MockLamp lamp2(2);

    Thermostat thermostat(&temperature, lamp1, lamp2, &targetTemperature);

    // First update: Lamp 1 should turn on
    thermostat.update();
    TEST_ASSERT_TRUE(lamp1.getStatus());
    TEST_ASSERT_FALSE(lamp2.getStatus());

    // Second update: Lamp 2 should turn on
    thermostat.update();
    TEST_ASSERT_FALSE(lamp1.getStatus());
    TEST_ASSERT_TRUE(lamp2.getStatus());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_thermostat_turns_off_lamps);
    RUN_TEST(test_thermostat_turns_on_lamps);
    RUN_TEST(test_thermostat_alternates_lamps);
    UNITY_END();
}

void loop() {
    // Not used in tests
}