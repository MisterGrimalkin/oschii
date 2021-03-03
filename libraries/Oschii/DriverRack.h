#ifndef DriverRack_h
#define DriverRack_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "Driver.h"

#define MAX_DRIVERS 512

#define ECHO_DRIVERS true

class DriverRack {
  public:
    DriverRack() {};
    String buildDrivers(JsonArray array);
    String buildDriver(JsonObject json);
    JsonArray toJson();
    String toPrettyJson();

  private:
    Driver * _drivers[MAX_DRIVERS];
    int _driverIndex;
};

#endif
