#ifndef SettingsService_h
#define SettingsService_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "FileService.h"

#define SETTINGS_FILENAME "/settings.json"

class SettingsService {
  public:
    SettingsService(FileService * files, String version, String buildDatetime);

    void load();
    void save();

    void set(String jsonString);

    void build(String json);
    String toPrettyJson();

    String getVersion();
    String getBuildDatetime();

    String getName();
    void setName(String name);

    bool isWifiEnabled();
    int getWifiTimeout();
    String getWifiName();
    String getWifiPassword();
    void setWifiEnabled(bool enabled);
    void setWifiTimeout(int timeout);
    void setWifiCredentials(String name, String password);

    bool isEthernetEnabled();
    int getEthernetTimeout();
    void setEthernetEnabled(bool enabled);
    void setEthernetTimeout(int timeout);

    int getOscPort();
    void setOscPort(int port);

    int getHttpPort();
    void setHttpPort(int port);
  private:
    FileService * _files;
    String _version, _buildDatetime, _name, _wifiName, _wifiPassword;
    bool _wifiEnabled, _ethernetEnabled;
    int _wifiTimeout, _ethernetTimeout, _oscPort, _httpPort;
};

#endif
