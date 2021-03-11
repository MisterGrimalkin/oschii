#ifndef HCSRSensor_h
#define HCSRSensor_h

#include "RangeSensor.h"
#include "I2CRack.h"

#define ECHO_TIMEOUT 5000

class HCSRSensor : public RangeSensor {
  public:
    HCSRSensor(I2CRack * i2cRack) : RangeSensor(i2cRack) {};

    virtual int getReading();

    virtual bool build(JsonObject json);
    virtual JsonObject toJson();
    virtual String toString();

  private:
    int _trigPin, _echoPin;
};

#endif
