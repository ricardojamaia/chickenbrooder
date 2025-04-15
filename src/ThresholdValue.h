#ifndef THRESHOLD_VALUE_H
#define THRESHOLD_VALUE_H

#include "StateValue.h"

class ThresholdValue : public StateValue {
public:
  ThresholdValue(float initialValue) : value(initialValue) {}

  void increase(float step) override {
    value += step;
    notifyListeners(value);
  }

  void decrease(float step) override {
    value -= step;
    notifyListeners(value);
  }

  float getValue() const override {
    return value;
  }

  void setValue(float newValue) override {
    value = newValue;
    notifyListeners(value);
  }

private:
  float value;
};

#endif