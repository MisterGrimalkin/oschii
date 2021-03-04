#ifndef DriverRack_h
#define DriverRack_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "Driver.h"
#include "GpioDriver.h"

#define MAX_DRIVERS 512

#define ECHO_DRIVERS true

class DriverRack {
  public:
    DriverRack() {};
    String buildDrivers(JsonArray array);
    String buildDriver(JsonObject json);
    JsonArray toJson();
    String toPrettyJson();

    void fireAll(int value);

    Driver * getDriver(String name);

  private:
    Driver * _drivers[MAX_DRIVERS];
    int _driverIndex;
};

#endif
