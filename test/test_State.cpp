#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "../src/State.h"

// Test: Initial Value
TEST_CASE("State initializes with the correct value", "[State]") {
  State<int> state(10);
  REQUIRE(state.getValue() == 10);
}

// Test: Set Value
TEST_CASE("State updates value correctly", "[State]") {
  State<int> state(10);
  state.setValue(20);
  REQUIRE(state.getValue() == 20);
}

// Test: No Notification for Same Value
TEST_CASE("State does not notify listeners when value does not change", "[State]") {
  State<int> state(10);
  bool listenerCalled = false;

  state.addListener([](int, void* context) {
    bool* called = static_cast<bool*>(context);
    *called = true;
  }, &listenerCalled);

  state.setValue(10); // Same value
  REQUIRE(listenerCalled == false);
}

// Test: Notify Listeners
TEST_CASE("State notifies listeners when value changes", "[State]") {
  State<int> state(10);
  bool listenerCalled = false;

  state.addListener([](int newValue, void* context) {
    bool* called = static_cast<bool*>(context);
    *called = true;
    REQUIRE(newValue == 20);
  }, &listenerCalled);

  state.setValue(20); // Change value
  REQUIRE(listenerCalled == true);
}

// Test: Increase Value
TEST_CASE("State increases value correctly", "[State]") {
  State<int> state(10);
  state.increase(5);
  REQUIRE(state.getValue() == 15);
}

// Test: Decrease Value
TEST_CASE("State decreases value correctly", "[State]") {
  State<int> state(10);
  state.decrease(3);
  REQUIRE(state.getValue() == 7);
}

// Test: Multiple Listeners
TEST_CASE("State notifies all listeners when value changes", "[State]") {
  State<int> state(10);
  bool listener1Called = false;
  bool listener2Called = false;

  state.addListener([](int, void* context) {
    bool* called = static_cast<bool*>(context);
    *called = true;
  }, &listener1Called);

  state.addListener([](int, void* context) {
    bool* called = static_cast<bool*>(context);
    *called = true;
  }, &listener2Called);

  state.setValue(20); // Change value
  REQUIRE(listener1Called == true);
  REQUIRE(listener2Called == true);
}

// Test: Listener Limit
TEST_CASE("State does not exceed maximum number of listeners", "[State]") {
  State<int> state(10);
  int listenerCount = 0;

  for (int i = 0; i < MAX_LISTENERS + 2; ++i) {
    state.addListener([](int, void* context) {
      int* count = static_cast<int*>(context);
      (*count)++;
    }, &listenerCount);
  }

  state.setValue(20); // Change value
  REQUIRE(listenerCount == MAX_LISTENERS);
}