#ifndef LAMP_H
#define LAMP_H

#include <Arduino.h>

class Lamp {
public:
  Lamp(uint8_t pin);

  virtual void begin();
  void turnOn();
  void turnOff();
  bool getStatus() const;

private:
  uint8_t pin;   // The pin connected to the lamp
  bool isOn;     // Internal status of the lamp (true = ON, false = OFF)
};

#endif // LAMP_H