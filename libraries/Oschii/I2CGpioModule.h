#ifndef I2CGpioModule_h
#define I2CGpioModule_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include <Wire.h>

#include "I2C.h"
#include "I2CModule.h"

#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))
#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))


class I2CGpioModule : public I2CModule {
  public:
    I2CGpioModule(I2C * i2c) : I2CModule(i2c) {};

    void setMode(int pin, bool output);
    void setResistance(int pin, bool up);

    bool read(int pin);
    void write(int pin, bool value);
    int portFor(int pin);
    void pull(int port);

    virtual bool build(JsonObject json);
};

#endif
