#ifndef AnalogSensor_h
#define AnalogSensor_h

#include "RangeSensor.h"

class AnalogSensor : public RangeSensor {
  public:
    AnalogSensor() : RangeSensor() {};
    AnalogSensor(int index) : RangeSensor(index) {};
    virtual bool build(JsonObject json);
    virtual String toString();
    virtual JsonObject toJson();
  private:
    int _pin;
    virtual int getReading();
};

#endif
