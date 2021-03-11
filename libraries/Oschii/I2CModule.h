#ifndef I2CModule_h
#define I2CModule_h

#include <stdlib.h>

#include <Wire.h>
#include "I2C.h"

#include <Arduino.h>
#include <ArduinoJson.h>

class I2CModule {
  public:
    I2CModule(I2C * i2c);

    virtual bool open(int address);
    virtual bool open(String address);
    virtual int readRegister(int registerAddress);
    virtual void writeRegister(int registerAddress, int data);

    virtual bool build(JsonObject json);
    String getName();

  protected:
    I2C * _i2c;
    String _name, _type, _chip;
    int _address;
};

#endif
