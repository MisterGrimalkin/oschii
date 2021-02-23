#ifndef RangeSensor_h
#define RangeSensor_h

#include "OSensor.h"

#include <Arduino.h>
#include <ArduinoJson.h>

#define MIN 0
#define MAX 1

#define MAX_SAMPLES 100

class RangeSensor : public OSensor {
  public:
    RangeSensor() : OSensor() {};
    RangeSensor(int index) : OSensor(index) {};

    virtual void readSensor();

    virtual bool build(JsonObject json);
    virtual void print() {};
    virtual int getReading() {};

  protected:
    int _samples, _sampleCount;
    bool _flipRange, _interleave;
    int _readingRange[2];
    int _valueRange[2];
    int _triggerBand[2];
    int _sampleBuffer[MAX_SAMPLES];

    int mapToValue(int reading);
    int capReading(int reading);
    int getMedianValue(int samples);
};

#endif