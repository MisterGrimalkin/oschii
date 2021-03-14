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
    ~Monitor();

    void update();

    bool build(JsonObject json);

    String toString();

    String getError();
    void setError(String error);

//    JsonObject toJson();

  private:
    SensorRack * _sensorRack;
    RemoteRack * _remoteRack;
    Sensor * _sensor;
    MonitorSendTo * _sendTos[MAX_MONITOR_REMOTES];
    int _sendToIndex, _pollInterval, _lastPolledAt;
    bool _onChange;
    String _error;

    ValueTransform * _transform;
    int applyTransform(int value);
    
//    StaticJsonDocument<128> _jsonRoot;
};

#endif
