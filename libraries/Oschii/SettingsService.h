#ifndef SettingsService_h
#define SettingsService_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "FileService.h"

class SettingsService {
  public:
    SettingsService(FileService files, String filename);

    void load();
    void save();

    void fromJson(String json);
    String toJson();

    String getName();
    void setName(String name);

    String getWifiSsid();
    void setWifiSsid(String ssid);

    String getWifiPassword();
    void setWifiPassword(String password);

    bool isEthernetPreferred();
    void setEthernetPreferred(bool preferEthernet);
  private:
    FileService _files;
    String _filename, _name, _wifiSsid, _wifiPassword;
    bool _preferEthernet;
};

#endif
