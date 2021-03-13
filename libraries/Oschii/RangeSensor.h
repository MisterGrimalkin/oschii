#ifndef RangeSensor_h
#define RangeSensor_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "Sensor.h"
#include "I2CRack.h"

#define MAX_SAMPLES 100

class RangeSensor : public Sensor {
  public:
    RangeSensor(I2CRack * i2cRack) : Sensor(i2cRack) {};

    virtual int getReading() {};

    virtual void readSensor();

    virtual bool build(JsonObject json);

    virtual String toString();

//    virtual JsonObject toJson();

  protected:
    int _sampleBuffer[MAX_SAMPLES];
    int _samples, _sampleCount;
    bool _interleave;

    int getMedianValue(int samples);
};

#endif
