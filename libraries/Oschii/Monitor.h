#ifndef Monitor_h
#define Monitor_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "SensorRack.h"
#include "RemoteRack.h"

#define MAX_MONITOR_REMOTES 128

class Monitor {
  public:
    Monitor(SensorRack * sensorRack, RemoteRack * remoteRack);

    bool build(String sensorName, JsonObject json);
    void update();

    JsonObject toJson();
    String toString();

  private:
    SensorRack * _sensorRack;
    RemoteRack * _remoteRack;
    Sensor * _sensor;
    Remote * _remotes[MAX_MONITOR_REMOTES];
    int _remoteIndex, _pollInterval, _lastPolledAt;
    bool _onChange;
};

#endif
