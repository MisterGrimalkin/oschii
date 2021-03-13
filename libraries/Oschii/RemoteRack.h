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
    ~RemoteRack();

    bool buildRemotes(JsonArray array);
    String buildRemote(JsonObject json);

    void loop();

    Remote * getRemote(String name);

//    JsonArray toJson();

  private:
    DriverRack * _driverRack;
    Remote * _remotes[MAX_REMOTES];
    int _remoteIndex;
//    StaticJsonDocument<128> _jsonRoot;
};

#endif
