#ifndef TouchSensor_h
#define TouchSensor_h

#include "BinarySensor.h"
#include "I2CRack.h"

class TouchSensor : public BinarySensor {
  public:
    TouchSensor(I2CRack * i2cRack) : BinarySensor(i2cRack) {};

    virtual bool getState();

    virtual bool build(JsonObject json);

    virtual String toString();

//    virtual JsonObject toJson();

  protected:
    int _pin, _triggerThreshold;
    bool _triggerHighPass;
};

#endif
