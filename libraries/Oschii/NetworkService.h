#ifndef NetworkService_h
#define NetworkService_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include <WiFi.h>
#include <ETH.h>

#include "SettingsService.h"

class NetworkService {
  public:
    NetworkService(SettingsService * settings);

    void connect();
    void disconnect();

    void loop();

    bool isConnected();

    String getIpAddress();

  private:
    SettingsService * _settings;
    bool _connected, _usingEthernet;

//    void WiFiEvent(WiFiEvent_t event);

//    void connectToEthernet();
    void connectToWifi();
};

#endif
