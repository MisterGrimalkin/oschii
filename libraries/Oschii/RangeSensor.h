#ifndef RangeSensor_h
#define RangeSensor_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "Sensor.h"
#include "ValueTransform.h"
#include "I2CRack.h"

#define MAX_SAMPLES 100

class RangeSensor : public Sensor {
  public:
    RangeSensor(I2CRack * i2cRack) : Sensor(i2cRack) {};

    virtual void readSensor();
    virtual int getReading() {};

    virtual bool build(JsonObject json);

    virtual JsonObject toJson();
    virtual String toString();

  protected:
    int _sampleBuffer[MAX_SAMPLES];
    int _samples, _sampleCount;
    bool _interleave;
    ValueTransform * _transform;

    int getMedianValue(int samples);
    int applyTransform(int value);
};

#endif
