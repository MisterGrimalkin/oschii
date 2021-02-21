#ifndef RangeSensor_h
#define RangeSensor_h

#include <Arduino.h>
#include <ArduinoJson.h>

#define MIN 0
#define MAX 1

#define MAX_SAMPLES 100

class RangeSensor {
  public:
    RangeSensor();
    RangeSensor(int index);

    void readSensor();
    bool hasChanged();
    int getValue();
    String getError();

    virtual bool build(JsonObject json);
    virtual void print() {};

  protected:
    int _index, _samples, _value, _sampleCount;
    bool _flipRange, _interleave, _changed;
    int _readingRange[2];
    int _valueRange[2];
    int _triggerBand[2];
    int _sampleBuffer[MAX_SAMPLES];
    String _error;

    int mapToValue(int reading);
    int capReading(int reading);
    int getMedianValue(int samples);

    virtual int getReading() {};
};

#endif