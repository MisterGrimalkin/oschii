#ifndef PwmDriver_h
#define PwmDriver_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "Driver.h"

class PwmDriver : public Driver {
  public:
    PwmDriver() : Driver() {};

    virtual void fire(int value);
    virtual bool build(JsonObject json);

    virtual JsonObject toJson();
    virtual String toString();

  private:
    int _pin, _channel;

};

#endif
