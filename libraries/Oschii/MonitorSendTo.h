#ifndef MonitorSendTo_h
#define MonitorSendTo_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "RemoteRack.h"
#include "ValueTransform.h"

class MonitorSendTo {
  public:
    MonitorSendTo(RemoteRack * remoteRack);
    ~MonitorSendTo();

    void send(int value);

    bool build(JsonObject json);

    String getAddress();

    String getError();
    void setError(String error);

//    JsonObject toJson();

  private:
    RemoteRack * _remoteRack;
    Remote * _remote;
    String _address;
    ValueTransform * _transform;
//    StaticJsonDocument<128> _jsonRoot;
    String _error;
};

#endif
