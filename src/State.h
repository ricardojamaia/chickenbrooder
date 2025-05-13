#ifndef STATE_H
#define STATE_H

#include <functional>
#include <type_traits>

template <typename T>
class State {
  public:
    State(T initialValue) : value(initialValue) {}

    T getValue() const { return value; }
    void setValue(T newValue) {
        value = newValue;
        if (listener) {
            listener(value); // Call the listener with the new value
        }
    }

    void addListener(std::function<void(T)> callback) {
        listener = callback;
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
    std::function<void(T)> listener = nullptr; // Listener for state changes
};

#endif // STATE_H