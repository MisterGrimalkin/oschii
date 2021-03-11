#ifndef AnalogSensor_h
#define AnalogSensor_h

#include "RangeSensor.h"
#include "I2CRack.h"

class AnalogSensor : public RangeSensor {
  public:
    AnalogSensor(I2CRack * i2cRack) : RangeSensor(i2cRack) {};

    virtual int getReading();

    virtual bool build(JsonObject json);
    virtual JsonObject toJson();
    virtual String toString();

  private:
    int _pin;
};

#endif
