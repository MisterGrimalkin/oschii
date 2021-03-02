#ifndef BinarySensor_h
#define BinarySensor_h

#include "Sensor.h"

class BinarySensor : public Sensor {
  public:
    BinarySensor() : Sensor() {};
    BinarySensor(int index) : Sensor(index) {};

    virtual void readSensor();
    virtual bool build(JsonObject json);

    virtual JsonObject toJson();
    virtual String toString();

    virtual bool getState() {};

  protected:
    int _onValue, _offValue, _bounceFilter, _holdOnFilter, _holdStarted;
    bool _invert, _lastState, _holding;
};

#endif
