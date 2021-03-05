#include "SettingsService.h"

SettingsService::SettingsService(FileService * files, String version, String buildDatetime) {
  _files = files;

  _version = version;
  _buildDatetime = buildDatetime;

  _name = "Oschii";

  _wifiEnabled = false;
  _wifiTimeout = 5000;
  _wifiName = "";
  _wifiPassword = "";

  _ethernetEnabled = false;
  _ethernetTimeout = 120000;

  _oscPort = 3333;
  _httpPort = 80;
}

void SettingsService::load() {
  String json = _files->readFile(SETTINGS_FILENAME);
  if (json == NULL ) {
    save();
    load();
    return;
  }
  build(json);
}

void SettingsService::save() {
  String json = toPrettyJson();
  _files->writeFile(SETTINGS_FILENAME, json);
}

void SettingsService::set(String json) {
  build(json);
  save();
  load();
}

void SettingsService::build(String json) {
  DynamicJsonDocument input(1024);
  DeserializationError error = deserializeJson(input, json.c_str());
  if (error) {
    Serial.println("Error parsing Settings JSON!");
  } else {
    if (input.containsKey("name")) _name = input["name"].as<String>();

    JsonObject networkJson = input["network"].as<JsonObject>();

    if (networkJson.containsKey("wifiEnabled"))     _wifiEnabled     = networkJson["wifiEnabled"];
    if (networkJson.containsKey("wifiTimeout"))     _wifiTimeout     = networkJson["wifiTimeout"];
    if (networkJson.containsKey("wifiName"))        _wifiName        = networkJson["wifiName"].as<String>();
    if (networkJson.containsKey("wifiPassword"))    _wifiPassword    = networkJson["wifiPassword"].as<String>();
    if (networkJson.containsKey("ethernetEnabled")) _ethernetEnabled = networkJson["ethernetEnabled"];
    if (networkJson.containsKey("ethernetTimeout")) _ethernetTimeout = networkJson["ethernetTimeout"];
    if (networkJson.containsKey("oscPort"))         _oscPort         = networkJson["oscPort"];
    if (networkJson.containsKey("httpPort"))        _httpPort        = networkJson["httpPort"];
  }
}

String SettingsService::toPrettyJson() {
  DynamicJsonDocument output(1024);
  output["name"] = _name;

  JsonObject networkJson = output.createNestedObject("network");

  networkJson["wifiEnabled"] = _wifiEnabled;
  networkJson["wifiTimeout"] = _wifiTimeout;
  networkJson["wifiName"] = _wifiName;
  networkJson["ethernetEnabled"] = _ethernetEnabled;
  networkJson["ethernetTimeout"] = _ethernetTimeout;
  networkJson["oscPort"] = _oscPort;
  networkJson["httpPort"] = _httpPort;

  String outputStr = "";
  serializeJsonPretty(output, outputStr);
  return outputStr;
}

String SettingsService::getVersion() {
  return _version;
}

String SettingsService::getBuildDatetime() {
  return _buildDatetime;
}

// Name

String SettingsService::getName() {
  return _name;
}

void SettingsService::setName(String name) {
  _name = name;
  save();
}

// Wifi

bool SettingsService::isWifiEnabled() {
  return _wifiEnabled;
}

int SettingsService::getWifiTimeout() {
  return _wifiTimeout;
}

String SettingsService::getWifiName() {
  return _wifiName;
}

String SettingsService::getWifiPassword() {
  return _wifiPassword;
}

void SettingsService::setWifiEnabled(bool enabled) {
  _wifiEnabled = enabled;
  save();
}

void SettingsService::setWifiTimeout(int timeout) {
  _wifiTimeout = timeout;
  save();
}

void SettingsService::setWifiCredentials(String name, String password) {
  _wifiName = name;
  _wifiPassword = password;
  save();
}

// Ethernet

bool SettingsService::isEthernetEnabled() {
  return _ethernetEnabled;
}

int SettingsService::getEthernetTimeout() {
  return _ethernetTimeout;
}

void SettingsService::setEthernetEnabled(bool enabled) {
  _ethernetEnabled = enabled;
  save();
}

void SettingsService::setEthernetTimeout(int timeout) {
  _ethernetTimeout = timeout;
  save();
}

// Ports

int SettingsService::getOscPort() {
  return _oscPort;
}

int SettingsService::getHttpPort() {
  return _httpPort;
}

void SettingsService::setOscPort(int port) {
  _oscPort = port;
  save();
}

void SettingsService::setHttpPort(int port) {
  _httpPort = port;
  save();
}




