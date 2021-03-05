#ifndef Oschii_h
#define Oschii_h

#define VERSION "0.0.2"
#define BUILD_DATETIME __DATE__ " " __TIME__

#include <Arduino.h>
#include <ArduinoJson.h>

#include "SettingsService.h"
#include "FileService.h"
#include "Racks.h"
#include "SerialAPI.h"

class Oschii {
  public:
    Oschii();
    void start();
    void loop();

  private:
    SettingsService * _settings;
    FileService * _files;
    Racks * _racks;
    SerialAPI * _serialAPI;
};

#endif
