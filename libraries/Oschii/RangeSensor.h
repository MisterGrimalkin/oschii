#ifndef RangeSensor_h
#define RangeSensor_h

#include "Sensor.h"

#include <Arduino.h>
#include <ArduinoJson.h>

#define MIN 0
#define MAX 1

#define MAX_SAMPLES 100

class RangeSensor : public Sensor {
  public:
    RangeSensor() : Sensor() {};

    virtual void readSensor();
    virtual int getReading() {};

    virtual bool build(JsonObject json);

    virtual JsonObject toJson();
    virtual String toString();

  protected:
    int _samples, _sampleCount, _lastReading;
    bool _flipRange, _interleave, _discardOutliers;

    int _readingRange[2];
    int _valueRange[2];
    int _bandPass[2];
    int _bandCut[2];
    int _sampleBuffer[MAX_SAMPLES];

    int mapToValue(int reading);
    int capReading(int reading);
    int getMedianValue(int samples);
};

#endif
