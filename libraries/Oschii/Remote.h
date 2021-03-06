#ifndef Remote_h
#define Remote_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "Driver.h"
#include "DriverRack.h"
#include "RemoteWriteTo.h"

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
    RemoteWriteTo * _writeTos[MAX_REMOTE_DRIVERS];
    int _writeToIndex;
    StaticJsonDocument<4096> _jsonRoot;
};

#endif
