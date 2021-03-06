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

    bool build(JsonObject json);
    void receive(int value);

    JsonObject toJson();
    String toString();

    String getAddress();

  private:
    DriverRack * _driverRack;
    String _address;
    int _driverIndex;
    int _offsets[MAX_REMOTE_DRIVERS];
    double _multipliers[MAX_REMOTE_DRIVERS];
    Driver * _drivers[MAX_REMOTE_DRIVERS];
    StaticJsonDocument<4096> _jsonRoot;
};

#endif
