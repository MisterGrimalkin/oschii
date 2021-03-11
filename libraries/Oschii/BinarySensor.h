#ifndef BinarySensor_h
#define BinarySensor_h

#include "Sensor.h"
#include "I2CRack.h"

class BinarySensor : public Sensor {
  public:
    BinarySensor(I2CRack * i2cRack) : Sensor(i2cRack) {};

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
