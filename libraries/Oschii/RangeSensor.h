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
    RangeSensor(int index) : Sensor(index) {};

    virtual void readSensor();

    virtual bool build(JsonObject json);
    virtual int getReading() {};
    virtual String toString();
    virtual JsonObject toJson();

  protected:
    int _samples, _sampleCount;
    bool _flipRange, _interleave, _bandCutActive;

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