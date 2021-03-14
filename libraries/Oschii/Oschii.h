#ifndef Oschii_h
#define Oschii_h

#define VERSION "0.0.2"
#define BUILD_DATETIME __DATE__ " " __TIME__

#include <Arduino.h>
#include <ArduinoJson.h>

//#include <ArduinoOSC.h>


#include "FileService.h"
#include "NetworkService.h"
#include "SettingsService.h"
#include "SerialAPI.h"
#include "Racks.h"

class Oschii {
  public:
    Oschii();

    void start();
    void loop();

  private:
    FileService * _files;
    NetworkService * _network;
    SettingsService * _settings;
    SerialAPI * _serialAPI;
    Racks * _racks;
};

#endif
