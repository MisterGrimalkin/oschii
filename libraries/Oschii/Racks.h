#ifndef Racks_h
#define Racks_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "I2CRack.h"
#include "SensorRack.h"
#include "DriverRack.h"
#include "RemoteRack.h"
#include "MonitorRack.h"

#define JSON_SIZE_LIMIT 15000

class Racks {
  public:
    Racks();

    bool buildConfig(String jsonString);
    bool buildConfig(JsonObject json);

    JsonObject toJson();
    String toPrettyJson();

    void start();
    void loop();

  private:
    I2CRack * _i2cRack;
    SensorRack * _sensorRack;
    DriverRack * _driverRack;
    RemoteRack * _remoteRack;
    MonitorRack * _monitorRack;

    StaticJsonDocument<JSON_SIZE_LIMIT> _jsonDoc;
};

#endif
