#ifndef NetworkService_h
#define NetworkService_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include <WiFi.h>
#include <ETH.h>

#include <WiFiUdp.h>
#include <OSCMessage.h>

#include <HTTPClient.h>

#include "SettingsService.h"

#define ECHO_SENDS true

class NetworkService {
  public:
    NetworkService(SettingsService * settings);

    void connect();
    void disconnect();

    void loop();

    bool isConnected();

    String getIpAddress();

    void sendOsc(String host, int port, String address, int value);
    void sendHttp(String method, String url, int value);

  private:
    SettingsService * _settings;
    bool _connected, _usingEthernet;

    WiFiUDP _udp;

//    void WiFiEvent(WiFiEvent_t event);

//    void connectToEthernet();
    void connectToWifi();
};

#endif
