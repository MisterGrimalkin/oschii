#ifndef MonitorRack_h
#define MonitorRack_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "Monitor.h"

#include "SensorRack.h"
#include "RemoteRack.h"

#define MAX_MONITORS 512

class MonitorRack {
  public:
    MonitorRack(SensorRack * sensorRack, RemoteRack * remoteRack);
    ~MonitorRack();

    bool buildMonitors(JsonArray array);
    String buildMonitor(JsonObject json);

    void loop();

//    JsonArray toJson();

  private:
    SensorRack * _sensorRack;
    RemoteRack * _remoteRack;
    Monitor * _monitors[MAX_MONITORS];
    int _monitorIndex;
//    StaticJsonDocument<128> _jsonRoot;
};

#endif
