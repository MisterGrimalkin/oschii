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

    void receive(int value);

    void update();

    bool build(JsonObject json);

    String toString();

    String getAddress();

//    JsonObject toJson();

  private:
    DriverRack * _driverRack;
    String _address;
    RemoteWriteTo * _writeTos[MAX_REMOTE_DRIVERS];
    int _writeToIndex;
//    StaticJsonDocument<128> _jsonRoot;
};

#endif
