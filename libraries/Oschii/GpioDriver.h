#ifndef GpioDriver_h
#define GpioDriver_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "Driver.h"
#include "I2CRack.h"

class GpioDriver : public Driver {
  public:
    GpioDriver(I2CRack * i2cRack) : Driver(i2cRack) {};

    virtual void fire(int value);

    virtual bool build(JsonObject json);

    virtual String toString();

//    virtual JsonObject toJson();

  private:
    I2CGpioModule * _i2cGpioModule;
    int _pin, _thresholdValue;
    bool _thresholdHighPass;
};

#endif