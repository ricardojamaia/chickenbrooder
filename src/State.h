#ifndef STATE_H
#define STATE_H

#include <type_traits> // For std::is_arithmetic

#define MAX_LISTENERS 5 // Maximum number of listeners

template <typename T>
class State {
public:
  State(T initialValue) : value(initialValue), listenerCount(0) {}

  T getValue() const {
    return value;
  }

  void setValue(T newValue) {
    if (newValue != value) {
      value = newValue;
      notifyListeners();
    }
  }

  void addListener(void (*listener)(T, void*), void* context) {
    if (listenerCount < MAX_LISTENERS) {
      listeners[listenerCount] = listener;
      contexts[listenerCount] = context;
      listenerCount++;
    }
  }

  // Enable `increase` and `decrease` only for numeric types
  template <typename U = T>
  typename std::enable_if<std::is_arithmetic<U>::value, void>::type increase(U step) {
    setValue(value + step);
  }

  template <typename U = T>
  typename std::enable_if<std::is_arithmetic<U>::value, void>::type decrease(U step) {
    setValue(value - step);
  }

private:
  T value;
  void (*listeners[MAX_LISTENERS])(T, void*); // Array of function pointers
  void* contexts[MAX_LISTENERS];             // Array of listener contexts
  int listenerCount;

  void notifyListeners() {
    for (int i = 0; i < listenerCount; ++i) {
      listeners[i](value, contexts[i]);
    }
  }
};

#endif // STATE_H