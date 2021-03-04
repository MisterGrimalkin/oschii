#ifndef RemoteRack_h
#define RemoteRack_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "Remote.h"
#include "DriverRack.h"

#define MAX_REMOTES 512

class RemoteRack {
  public:
    RemoteRack(DriverRack * driverRack);
    String buildRemotes(JsonObject json);
    String buildRemote(String name, JsonObject json);

    Remote * getRemote(String name);

  private:
    DriverRack * _driverRack;
    Remote * _remotes[MAX_REMOTES];
    int _remoteIndex;
};

#endif
