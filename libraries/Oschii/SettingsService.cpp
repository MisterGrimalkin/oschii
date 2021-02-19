#include "SettingsService.h"

SettingsService::SettingsService(FileService files, String filename) {
  _files = files;
  _filename = filename;

  _name = "Oschii";
  _wifiSsid = "";
  _wifiPassword = "";
  _preferEthernet = false;
}

void SettingsService::load() {
  String json = _files.readFile(_filename);
  if (json==NULL ) {
    save();
    load();
    return;
  }
  fromJson(json);
}
void SettingsService::save() {
  String json = toJson();
  _files.writeFile(_filename, json);
}


void SettingsService::fromJson(String json) {
  DynamicJsonDocument input(1024);
  DeserializationError error = deserializeJson(input, json.c_str());
  if (error) {
    Serial.println("Error parsing Settings JSON!");
  } else {
    if (input.containsKey("name"))            _name = input["name"].as<String>();
    if (input.containsKey("wifiSsid"))        _wifiSsid = input["wifiSsid"].as<String>();
    if (input.containsKey("wifiPassword"))    _wifiPassword = input["wifiPassword"].as<String>();
    if (input.containsKey("preferEthernet"))  _preferEthernet = input["preferEthernet"];
  }
}
String SettingsService::toJson() {
  DynamicJsonDocument output(1024);
  output["name"] = _name;
  output["wifiSsid"] = _wifiSsid;
  output["wifiPassword"] = _wifiPassword;
  output["preferEthernet"] = _preferEthernet;

  String outputStr = "";
  serializeJsonPretty(output, outputStr);
  return outputStr;
}

String SettingsService::getName() {
  return _name;
}
void SettingsService::setName(String name) {
  _name = name;
  save();
}

String SettingsService::getWifiSsid() {
  return _wifiSsid;
}
void SettingsService::setWifiSsid(String ssid) {
  _wifiSsid = ssid;
  save();
}

String SettingsService::getWifiPassword() {
  return _wifiPassword;
}
void SettingsService::setWifiPassword(String password) {
  _wifiPassword = password;
  save();
}

bool SettingsService::isEthernetPreferred() {
  return _preferEthernet;
}
void SettingsService::setEthernetPreferred(bool preferEthernet) {
  _preferEthernet = preferEthernet;
  save();
}




