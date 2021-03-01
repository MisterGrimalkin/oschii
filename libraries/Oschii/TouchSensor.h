#ifndef TouchSensor_h
#define TouchSensor_h

#include "BinarySensor.h"

class TouchSensor : public BinarySensor {
  public:
    TouchSensor() : BinarySensor() {};
    TouchSensor(int index) : BinarySensor(index) {};

    virtual bool getState();

    virtual bool build(JsonObject json);

    virtual JsonObject toJson();
    virtual String toString();

  protected:
    int _pin, _triggerThreshold;
    bool _triggerHighPass;
};

#endif
