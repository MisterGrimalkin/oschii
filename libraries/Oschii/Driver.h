#ifndef Driver_h
#define Driver_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "ValueTransform.h"
#include "I2CRack.h"

class Driver {
  public:
    Driver(I2CRack * i2cRack);

    virtual void fire(int value) {};

    virtual bool build(JsonObject json);

    virtual String toString();

    int getValue();
    String getType();
    String getName();
    String getError();

//    String toPrettyJson();
//    virtual JsonObject toJson();

  protected:
    I2CRack * _i2cRack;
    I2CModule * _i2cModule;
    ValueTransform * _transform;

    int _value, _initialValue;
    bool _built, _invert;
    String _name, _type, _error;

    int applyTransform(int value);
    void setError(String error);

//    StaticJsonDocument<128> _jsonRoot;
};

#endif
