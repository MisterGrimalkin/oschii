#ifndef SerialAPI_h
#define SerialAPI_h

#include <Arduino.h>

#include "SettingsService.h"
#include "NetworkService.h"
#include "Racks.h"

class SerialAPI {
  public:
    SerialAPI(SettingsService * settings, NetworkService * network, Racks * racks);

    void start();
    void loop();

    void processInput(String input);

    String prompt(String prompt);
    String read();

  private:
    SettingsService * _settings;
    NetworkService * _network;
    Racks * _racks;
};

#endif