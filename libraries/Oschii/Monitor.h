#ifndef Monitor_h
#define Monitor_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "SensorRack.h"
#include "RemoteRack.h"
#include "MonitorSendTo.h"

#define ECHO_MONITOR_SENDS false

#define MAX_MONITOR_REMOTES 128

class Monitor {
  public:
    Monitor(SensorRack * sensorRack, RemoteRack * remoteRack);

    void update();

    bool build(JsonObject json);

    String toString();

//    JsonObject toJson();

  private:
    SensorRack * _sensorRack;
    RemoteRack * _remoteRack;
    Sensor * _sensor;
    MonitorSendTo * _sendTos[MAX_MONITOR_REMOTES];
    int _sendToIndex, _pollInterval, _lastPolledAt;
    bool _onChange;
//    StaticJsonDocument<128> _jsonRoot;
};

#endif
