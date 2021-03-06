#ifndef Monitor_h
#define Monitor_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "SensorRack.h"
#include "RemoteRack.h"
#include "MonitorSendTo.h"

#define MAX_MONITOR_REMOTES 128

class Monitor {
  public:
    Monitor(SensorRack * sensorRack, RemoteRack * remoteRack);

    bool build(JsonObject json);
    void update();

    JsonObject toJson();
    String toString();

  private:
    SensorRack * _sensorRack;
    RemoteRack * _remoteRack;
    Sensor * _sensor;
    MonitorSendTo * _sendTos[MAX_MONITOR_REMOTES];
    int _sendToIndex, _pollInterval, _lastPolledAt;
    bool _onChange;
    StaticJsonDocument<4096> _jsonRoot;
};

#endif
