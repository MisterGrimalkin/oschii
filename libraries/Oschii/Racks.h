#ifndef Racks_h
#define Racks_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "I2CRack.h"
#include "SensorRack.h"
#include "DriverRack.h"
#include "RemoteRack.h"
#include "MonitorRack.h"
#include "FileService.h"

#define JSON_SIZE_LIMIT 15000
#define CONFIG_FILE "/oschii/config.json"
#define RESTART_TIMEOUT 3

class Racks {
  public:
    Racks(FileService * files);

    void init();
    void destroy();

    String getSavedConfig();

    bool buildConfig(String jsonString, bool save);
    bool buildConfig(JsonObject json);

    void start();
    void loop();

    void restartESP();

//    JsonObject toJson();
//    String toPrettyJson();

  private:
    I2CRack * _i2cRack;
    SensorRack * _sensorRack;
    DriverRack * _driverRack;
    RemoteRack * _remoteRack;
    MonitorRack * _monitorRack;
    FileService * _files;

    StaticJsonDocument<JSON_SIZE_LIMIT> _jsonDoc;
};

#endif
