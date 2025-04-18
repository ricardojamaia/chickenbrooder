#ifndef LAMP_H
#define LAMP_H

#include <Arduino.h>

class Lamp {
public:
  Lamp(uint8_t pin);

  virtual void begin();
  virtual void turnOn();
  virtual void turnOff();
  bool getStatus() const;

protected:
  bool isOn;     // Internal status of the lamp (true = ON, false = OFF)
private:
  uint8_t pin;   // The pin connected to the lamp
};

#endif // LAMP_H