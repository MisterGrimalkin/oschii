#ifndef GpioDriver_h
#define GpioDriver_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "Driver.h"

class GpioDriver : public Driver {
  public:
    GpioDriver() : Driver() {};

    virtual void fire(int value);
    virtual bool build(JsonObject json);

    virtual JsonObject toJson();
    virtual String toString();

  private:
    int _pin, _thresholdValue;
    bool _thresholdHighPass, _invert;
};

#endif