#ifndef MonitorSendTo_h
#define MonitorSendTo_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "RemoteRack.h"
#include "ValueTransform.h"

class MonitorSendTo {
  public:
    MonitorSendTo(RemoteRack * remoteRack);

    bool build(JsonObject json);
    void send(int value);

    JsonObject toJson();
    String getAddress();

  private:
    RemoteRack * _remoteRack;
    Remote * _remote;
    StaticJsonDocument<128> _jsonRoot;
    String _address;
    ValueTransform * _transform;
};

#endif