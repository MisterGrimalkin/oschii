#ifndef Sensor_h
#define Sensor_h

#include <Arduino.h>
#include <ArduinoJson.h>

class Sensor {
  public:
    Sensor();

    virtual void readSensor() {};
    virtual bool build(JsonObject json);

    String toPrettyJson();
    virtual JsonObject toJson();
    virtual String toString();

    int getValue();
    bool hasChanged();
    String getType();
    String getName();
    String getError();

  protected:
    int _value, _lastChanged;
    bool _built, _changed;
    String _name, _type, _error;
    StaticJsonDocument<128> _jsonRoot;

    void setError(String error);
};

#endif
