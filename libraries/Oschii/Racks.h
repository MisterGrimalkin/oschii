#ifndef Racks_h
#define Racks_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "SensorRack.h"
#include "DriverRack.h"
#include "RemoteRack.h"
#include "MonitorRack.h"

#define JSON_SIZE_LIMIT 8192

class Racks {
  public:
    Racks();

    bool buildConfig(String jsonString);
    bool buildConfig(JsonObject json);

    bool buildScene(String jsonString);
    bool buildScene(JsonObject json);

    void start();
    void loop();

  private:
    SensorRack * _sensorRack;
    DriverRack * _driverRack;
    RemoteRack * _remoteRack;
    MonitorRack * _monitorRack;

    StaticJsonDocument<JSON_SIZE_LIMIT> _jsonDoc;
};

#endif
