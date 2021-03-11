#ifndef GpioSensor_h
#define GpioSensor_h

#include "BinarySensor.h"
#include "I2CRack.h"
#include "I2CGpioModule.h"

class GpioSensor : public BinarySensor {
  public:
    GpioSensor(I2CRack * i2cRack) : BinarySensor(i2cRack) {};

    virtual bool getState();

    virtual bool build(JsonObject json);

    virtual JsonObject toJson();
    virtual String toString();

  protected:
    I2CGpioModule * _i2cGpioModule;
    int _pin;
    String _resistor;
};

#endif
