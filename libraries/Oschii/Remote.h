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

    String getError();
    void setError(String error);

//    JsonObject toJson();

  private:
    DriverRack * _driverRack;
    String _address;
    RemoteWriteTo * _writeTos[MAX_REMOTE_DRIVERS];
    int _writeToIndex;
    String _error;
//    StaticJsonDocument<128> _jsonRoot;
};

#endif
