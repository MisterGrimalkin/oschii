#ifndef PwmDriver_h
#define PwmDriver_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "Driver.h"
#include "I2CRack.h"

class PwmDriver : public Driver {
  public:
    PwmDriver(I2CRack * i2cRack) : Driver(i2cRack) {};

    virtual void fire(int value);
    virtual bool build(JsonObject json);

    virtual JsonObject toJson();
    virtual String toString();

  private:
    I2CRack * _i2cRack;
    I2CModule * _i2cModule;

    int _pin, _channel;

};

#endif
