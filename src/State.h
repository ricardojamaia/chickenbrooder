#ifndef STATE_H
#define STATE_H

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

  bool addListener(void (*listener)(T, void*), void* context) {
    if (listenerCount < MAX_LISTENERS) {
      listeners[listenerCount] = listener;
      contexts[listenerCount] = context;
      listenerCount++;
      return true; // Successfully added
    }
    return false; // Failed to add listener (array full)
  }

private:
  T value;
  void (*listeners[MAX_LISTENERS])(T, void*); // Array of function pointers
  void* contexts[MAX_LISTENERS];             // Array of listener contexts
  int listenerCount;

  void notifyListeners() {
    for (int i = 0; i < listenerCount; ++i) {
      listeners[i](value, contexts[i]); // Call each listener with the value and context
    }
  }
};

#endif // STATE_H