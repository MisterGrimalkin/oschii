#ifndef Oschii_h
#define Oschii_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "SettingsService.h"
#include "FileService.h"

#include "SensorRack.h"
#include "DriverRack.h"
#include "RemoteRack.h"
#include "MonitorRack.h"

class Oschii {
  public:
    Oschii();

    bool buildConfig(JsonObject json);
    bool buildScene(JsonObject json);

    void loop();

  private:
    SensorRack * _sensorRack;
    DriverRack * _driverRack;
    RemoteRack * _remoteRack;
    MonitorRack * _monitorRack;
};

#endif
