#ifndef OSensor_h
#define OSensor_h

#include <Arduino.h>
#include <ArduinoJson.h>

class OSensor {
  public:
    OSensor();
    OSensor(int index);

    int getValue();
    bool hasChanged();
    String getType();
    String getName();
    String getError();

    virtual void readSensor() {};

    virtual bool build(JsonObject json);
    virtual void print() {};
    virtual int getReading() {};

  protected:
    int _index, _value;
    bool _built, _changed;
    String _name, _type, _error;
};

#endif
