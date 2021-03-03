#ifndef AnalogSensor_h
#define AnalogSensor_h

#include "RangeSensor.h"

class AnalogSensor : public RangeSensor {
  public:
    AnalogSensor() : RangeSensor() {};

    virtual int getReading();

    virtual bool build(JsonObject json);
    virtual JsonObject toJson();
    virtual String toString();

  private:
    int _pin;
};

#endif
