#ifndef HCSRSensor_h
#define HCSRSensor_h

#include "RangeSensor.h"

class HCSRSensor : public RangeSensor {
  public:
    HCSRSensor() : RangeSensor() {};

    virtual int getReading();

    virtual bool build(JsonObject json);
    virtual JsonObject toJson();
    virtual String toString();

  private:
    int _trigPin, _echoPin;
};

#endif
