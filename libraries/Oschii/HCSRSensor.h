#ifndef HCSRSensor_h
#define HCSRSensor_h

#include "RangeSensor.h"

class HCSRSensor : public RangeSensor {
  public:
    HCSRSensor() : RangeSensor() {};
    HCSRSensor(int index) : RangeSensor(index) {};
    virtual bool build(JsonObject json);
    virtual void print();
  private:
    int _trigPin, _echoPin;
    virtual int getReading();
};

#endif
