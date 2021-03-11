#ifndef I2CRack_h
#define I2CRack_h

#include <Arduino.h>
#include <Wire.h>

#include "I2C.h"
#include "I2CModule.h"
#include "I2CGpioModule.h"
#include "I2CPwmModule.h"

class I2CRack {
  public:
    I2CRack() {};

    bool build(JsonObject json);

    I2CModule * getModule(String name);

    I2C * getI2C();

  private:
    I2C * _i2c;
    I2CModule * _modules[];
    int _moduleIndex;

};

#endif