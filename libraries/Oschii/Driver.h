#ifndef Driver_h
#define Driver_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "I2CRack.h"

class Driver {
  public:
    Driver(I2CRack * i2cRack);

    virtual void fire(int value) {};
    virtual bool build(JsonObject json);

    String toPrettyJson();
    virtual JsonObject toJson();
    virtual String toString();

    int getValue();
    String getType();
    String getName();
    String getError();

  protected:
    I2CRack * _i2cRack;
    I2CModule * _i2cModule;

    int _value, _initialValue;
    bool _built;
    String _name, _type, _error;
    StaticJsonDocument<128> _jsonRoot;

    void setError(String error);
};

#endif
