#ifndef I2CPwmModule_h
#define I2CPwmModule_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_PWMServoDriver.h>

#include "I2C.h"
#include "I2CModule.h"

class I2CPwmModule : public I2CModule {
  public:
    I2CPwmModule(I2C * i2c) : I2CModule(i2c) {};

    virtual bool open(int address);
    virtual bool build(JsonObject json);

    void write(int pin, int value, bool invert);

   private:
     Adafruit_PWMServoDriver * _pwm;

};

#endif
