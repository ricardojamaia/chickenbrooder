#ifndef STATE_VALUE_H
#define STATE_VALUE_H

#define MAX_LISTENERS 5 // Maximum number of listeners

class StateValue {
public:
  virtual void increase(float step) = 0;
  virtual void decrease(float step) = 0;
  virtual float getValue() const = 0;
  virtual void setValue(float value) = 0;
  virtual ~StateValue() {}

  // Listener registration
  void addListener(void (*listener)(float)) {
    for (int i = 0; i < MAX_LISTENERS; i++) {
      if (listeners[i] == nullptr) {
        listeners[i] = listener;
        return;
      }
    }
  }

protected:
  void notifyListeners(float newValue) {
    for (int i = 0; i < MAX_LISTENERS; i++) {
      if (listeners[i] != nullptr) {
        listeners[i](newValue);
      }
    }
  }

private:
  void (*listeners[MAX_LISTENERS])(float) = {nullptr}; // Array of listener function pointers
};

#endif