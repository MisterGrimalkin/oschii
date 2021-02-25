#ifndef HCSRSensor_h
#define HCSRSensor_h

#include "RangeSensor.h"

class HCSRSensor : public RangeSensor {
  public:
    HCSRSensor() : RangeSensor() {};
    HCSRSensor(int index) : RangeSensor(index) {};
    virtual bool build(JsonObject json);
    virtual String toString();
    virtual JsonObject toJson();
  private:
    int _trigPin, _echoPin;
    virtual int getReading();
};

#endif
