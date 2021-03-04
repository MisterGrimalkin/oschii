#ifndef Oschii_h
#define Oschii_h

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
    Racks * _racks;
    SerialAPI * _serial;
};

#endif
