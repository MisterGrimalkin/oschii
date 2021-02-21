#ifndef HCSRSensor_h
#define HCSRSensor_h

#include <Arduino.h>
#include <ArduinoJson.h>

#define MIN 0
#define MAX 1

#define MAX_SAMPLES 100

class HCSRSensor {
  public:
    HCSRSensor();
    HCSRSensor(int index);
    bool build(JsonObject json);
    String getError();
    void readSensor();
    bool hasChanged();
    int getValue();
    void print();
  private:
    int _index, _trigPin, _echoPin, _samples, _value;
    int _readingRange[2];
    int _valueRange[2];
    int _triggerBand[2];
    int _sampleBuffer[MAX_SAMPLES];
    int _sampleCount;
    bool _flipRange, _interleave, _changed;
    String _error;
    int mapToValue(int reading);
    int capReading(int reading);
    int getReading();
    int getMedianValue(int samples);
};

#endif
