#ifndef Sensor_h
#define Sensor_h

#include <Arduino.h>
#include <ArduinoJson.h>

class Sensor {
  public:
    Sensor();
    Sensor(int index);

    int getValue();
    bool hasChanged();
    String getType();
    String getName();
    String getError();

    virtual void readSensor() {};

    virtual bool build(JsonObject json);
    virtual int getReading() {};
    virtual String toString();
    virtual JsonObject toJson();
    String toPrettyJson();

  protected:
    int _index, _value;
    bool _built, _changed;
    String _name, _type, _error;
};

#endif
