#ifndef STATE_H
#define STATE_H

#include <functional>
#include <type_traits>
#include "BrooderLog.h"

#define MAX_LISTENERS 5

template <typename T>
class State {
  public:
    State(T initialValue) : value(initialValue) {}

    T getValue() const { return value; }
    void setValue(T newValue) {
        if (newValue == value) return; // No change, no need to update and notify

        value = newValue;
        if (listenerCount > 0) {
            for (int i = 0; i < listenerCount; ++i) {
                listeners[i](value); // Notify all listeners
            }
        }
    }

    void addListener(std::function<void(T)> callback) {
        if (listenerCount < MAX_LISTENERS) {
            listeners[listenerCount++] = callback;
        } else {
            // Handle the case where the maximum number of listeners is reached
            BROODER_LOG_E("Max listeners reached.");
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
    std::function<void(T)> listeners[MAX_LISTENERS];
    int listenerCount = 0;
};

#endif // STATE_H