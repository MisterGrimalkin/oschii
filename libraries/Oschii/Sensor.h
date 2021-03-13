#ifndef Sensor_h
#define Sensor_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "I2C.h"
#include "I2CRack.h"
#include "I2CModule.h"

class Sensor {
  public:
    Sensor(I2CRack * i2cRack);

    virtual void readSensor() {};

    virtual bool build(JsonObject json);

    virtual String toString();

    int getValue();
    bool hasChanged();
    String getType();
    String getName();
    String getError();

//    String toPrettyJson();
//    virtual JsonObject toJson();

  protected:
    I2CRack * _i2cRack;
    I2CModule * _i2cModule;

    int _value, _lastChanged;
    bool _built, _changed;
    String _name, _type, _error;

    void setError(String error);

//    StaticJsonDocument<128> _jsonRoot;
};

#endif
