#include "Racks.h"

Racks::Racks(FileService * files) {
  init();
  _files = files;
}

void Racks::init() {
  _i2cRack = new I2CRack();
  _sensorRack = new SensorRack(_i2cRack);
  _driverRack = new DriverRack(_i2cRack);
  _remoteRack = new RemoteRack(_driverRack);
  _monitorRack = new MonitorRack(_sensorRack, _remoteRack);
}

void Racks::destroy() {
  if ( _i2cRack != NULL ) delete _i2cRack;
  if ( _sensorRack != NULL ) delete _sensorRack;
  if ( _driverRack != NULL ) delete _driverRack;
  if ( _remoteRack != NULL ) delete _remoteRack;
  if ( _monitorRack != NULL ) delete _monitorRack;
}

String Racks::getSavedConfig() {
  return _files->readFile(CONFIG_FILE);
}

void Racks::start() {
  String configStr = getSavedConfig();
  if ( configStr == "" ) {
    Serial.println("> No saved configuration\n");
  } else {
    buildConfig(configStr, false);
  }
}

void Racks::loop() {
  _monitorRack->loop();
  _remoteRack->loop();
}

bool Racks::buildConfig(String jsonString, bool save) {
  const char * json = jsonString.c_str();
  DeserializationError error = deserializeJson(_jsonDoc, json);
  if ( error ) return false;
  JsonObject obj = _jsonDoc.as<JsonObject>();
  bool success = buildConfig(obj);
  if ( success && save ) {
    _files->writeFile(CONFIG_FILE, jsonString);
    restartESP();
  }
  return success;
}

bool Racks::buildConfig(JsonObject json) {
  destroy();

  int freeHeapSize = esp_get_free_heap_size();

  init();

  Serial.println("> Building CONFIGURATION....\n");

  if ( json.containsKey("i2c") ) {
    JsonObject i2cJson = json["i2c"];
    _i2cRack->build(i2cJson);
  }

  JsonArray sensorArray = json["sensors"];
  JsonArray driverArray = json["drivers"];
  JsonArray remoteArray = json["driverRemotes"];
  JsonArray monitorArray = json["sensorMonitors"];

  _sensorRack->buildSensors(sensorArray);
  _driverRack->buildDrivers(driverArray);
  _remoteRack->buildRemotes(remoteArray);
  _monitorRack->buildMonitors(monitorArray);

  Serial.print("Configuration uses ");
  Serial.print(freeHeapSize - esp_get_free_heap_size());
  Serial.print(" bytes (free space: ");
  Serial.print(esp_get_free_heap_size());
  Serial.println(" bytes)\n");

  Serial.println("> OK\n");

  return true;
}

void Racks::restartESP() {
  Serial.print("\nRestarting Oschii in ");
  for ( int i=RESTART_TIMEOUT; i>0; i-- ) {
    Serial.print(i);
    Serial.print(" ");
    delay(1000);
  }
  Serial.println("\n!RESTARTING!\n\n");
  ESP.restart();
}

//JsonObject Racks::toJson() {
//  _jsonDoc.clear();
//  JsonObject json = _jsonDoc.createNestedObject("config");
//  json["sensors"] = _sensorRack->toJson();
//  json["drivers"] = _driverRack->toJson();
//  json["driverRemotes"] = _remoteRack->toJson();
//  json["sensorMonitors"] = _monitorRack->toJson();
//  return json;
//}
//
//String Racks::toPrettyJson() {
//  String outputStr = "";
//  serializeJsonPretty(toJson(), outputStr);
//  return outputStr;
//}
