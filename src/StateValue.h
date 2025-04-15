#ifndef STATE_VALUE_H
#define STATE_VALUE_H

#define MAX_LISTENERS 5 // Maximum number of listeners

class StateValue {
public:
  StateValue(float initialValue) : value(initialValue), listenerCount(0) {}

  void increase(float step) {
    value += step;
    notifyListeners();
  }

  void decrease(float step) {
    value -= step;
    notifyListeners();
  }

  float getValue() const {
    return value;
  }

  void setValue(float newValue) {
    if (newValue != value) {
      value = newValue;
      notifyListeners();
    }
  }

  void addListener(void (*listener)(float, void*), void* context) {
    if (listenerCount < MAX_LISTENERS) {
      listeners[listenerCount] = listener;
      contexts[listenerCount] = context;
      listenerCount++;
    }
  }

private:
  float value;
  void (*listeners[MAX_LISTENERS])(float, void*); // Array of function pointers
  void* contexts[MAX_LISTENERS];                 // Array of listener contexts
  int listenerCount;

  void notifyListeners() {
    for (int i = 0; i < listenerCount; ++i) {
      listeners[i](value, contexts[i]);
    }
  }
};

#endif