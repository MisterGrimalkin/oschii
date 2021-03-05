#ifndef Remote_h
#define Remote_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "Driver.h"
#include "DriverRack.h"

#define MAX_REMOTE_DRIVERS 128

class Remote {
  public:
    Remote(DriverRack * driverRack);

    bool build(String name, JsonObject json);
    void receive(int value);

    JsonObject toJson();
    String toString();

    String getName();

  private:
    DriverRack * _driverRack;
    String _name;
    int _driverIndex;
    int _offsets[MAX_REMOTE_DRIVERS];
    double _multipliers[MAX_REMOTE_DRIVERS];
    Driver * _drivers[MAX_REMOTE_DRIVERS];
    StaticJsonDocument<512> _jsonRoot;
};

#endif
