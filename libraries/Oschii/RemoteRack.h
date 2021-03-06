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
    String buildRemotes(JsonArray array);
    String buildRemote(JsonObject json);

    JsonArray toJson();

    Remote * getRemote(String name);

  private:
    DriverRack * _driverRack;
    Remote * _remotes[MAX_REMOTES];
    int _remoteIndex;
    StaticJsonDocument<4096> _jsonRoot;
};

#endif
