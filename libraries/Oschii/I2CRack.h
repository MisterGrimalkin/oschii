#ifndef I2CRack_h
#define I2CRack_h

#include <Arduino.h>
#include <Wire.h>

#include "I2C.h"
#include "I2CModule.h"
#include "I2CGpioModule.h"
#include "I2CPwmModule.h"

#define MAX_MODULES 64

class I2CRack {
  public:
    I2CRack() {};

    bool build(JsonObject json);

    I2CModule * getModule(String name);

    I2C * getI2C();

    String getError();
    void setError(String error);

  private:
    I2C * _i2c;
    I2CModule * _modules[MAX_MODULES];
    int _moduleIndex;
    String _error;
};

#endif