#ifndef SerialAPI_h
#define SerialAPI_h

#include <Arduino.h>

#include "Racks.h"
#include "SettingsService.h"

class SerialAPI {
  public:
    SerialAPI(SettingsService * settings, Racks * racks); //, NetworkService * network);

    void start();
    void loop();

    void processInput(String input);

    String prompt(String prompt);
    String read();

  private:
    Racks * _racks;
    SettingsService * _settings;
//    NetworkService * _network;
};

#endif