#ifndef AnalogSensor_h
#define AnalogSensor_h

#include "RangeSensor.h"

class AnalogSensor : public RangeSensor {
  public:
    AnalogSensor() : RangeSensor() {};
    AnalogSensor(int index) : RangeSensor(index) {};
    virtual bool build(JsonObject json);
    virtual void print();
  private:
    int _pin;
    virtual int getReading();
};

#endif
