#ifndef THRESHOLD_VALUE_H
#define THRESHOLD_VALUE_H

#include "StateValue.h"

class ThresholdValue : public StateValue {
public:
  ThresholdValue(float initialValue) : value(initialValue) {}
  void increase(float step) override { value += step; }
  void decrease(float step) override { value -= step; }
  float getValue() const override { return value; }
  void setValue(float newValue) override { value = newValue; }

private:
  float value;
};

#endif