#ifndef RemoteWriteTo_h
#define RemoteWriteTo_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "DriverRack.h"
#include "ValueTransform.h"

class RemoteWriteTo {
  public:
    RemoteWriteTo(DriverRack * driverRack);

    bool build(JsonObject json);
    void write(int value);

    JsonObject toJson();
    String toString();

    String getDriverName();

  private:
    DriverRack * _driverRack;
    Driver * _driver;
    StaticJsonDocument<4096> _jsonRoot;
    ValueTransform * _transform;
};

#endif
