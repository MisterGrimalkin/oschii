#ifndef DriverRack_h
#define DriverRack_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "Driver.h"
#include "GpioDriver.h"
#include "PwmDriver.h"
#include "I2CRack.h"

#define MAX_DRIVERS 512

#define ECHO_DRIVERS true

class DriverRack {
  public:
    DriverRack(I2CRack * i2cRack);
    String buildDrivers(JsonArray array);
    String buildDriver(JsonObject json);
    JsonArray toJson();
    String toPrettyJson();

    void fireAll(int value);

    Driver * getDriver(String name);

  private:
    StaticJsonDocument<128> _jsonRoot;
    Driver * _drivers[MAX_DRIVERS];
    int _driverIndex;
    I2CRack * _i2cRack;
};

#endif
