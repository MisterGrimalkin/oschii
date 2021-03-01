#ifndef GpioSensor_h
#define GpioSensor_h

#include "BinarySensor.h"

class GpioSensor : public BinarySensor {
  public:
    GpioSensor() : BinarySensor() {};
    GpioSensor(int index) : BinarySensor(index) {};

    virtual bool getState();

    virtual bool build(JsonObject json);

    virtual JsonObject toJson();
    virtual String toString();

  protected:
    int _pin;
    String _resistor;
};

#endif
