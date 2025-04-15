#ifndef STATE_VALUE_H
#define STATE_VALUE_H

class StateValue {
public:
  virtual void increase(float step) = 0;
  virtual void decrease(float step) = 0;
  virtual float getValue() const = 0;
  virtual void setValue(float value) = 0;
  virtual ~StateValue() {}
};

#endif