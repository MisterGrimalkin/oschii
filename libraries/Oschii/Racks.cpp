#include "Racks.h"

Racks::Racks() {
  _sensorRack = new SensorRack();
  _driverRack = new DriverRack();
  _remoteRack = new RemoteRack(_driverRack);
  _monitorRack = new MonitorRack(_sensorRack, _remoteRack);
}

void Racks::start() {
}

void Racks::loop() {
//  _sensorRack->readSensors();
  _monitorRack->loop();
}

bool Racks::buildConfig(String jsonString) {
  const char * json = jsonString.c_str();
  DeserializationError error = deserializeJson(_jsonDoc, json);
  if ( error ) return false;
  JsonObject obj = _jsonDoc.as<JsonObject>();
  return buildConfig(obj);
}

bool Racks::buildConfig(JsonObject json) {
  Serial.println("> Building CONFIGURATION....\n");

  JsonArray sensorArray = json["sensors"];
  _sensorRack->buildSensors(sensorArray);

  JsonArray driverArray = json["drivers"];
  _driverRack->buildDrivers(driverArray);

  Serial.println("> OK\n");

  return true;
}

bool Racks::buildScene(String jsonString) {
  const char * json = jsonString.c_str();
  DeserializationError error = deserializeJson(_jsonDoc, json);
  if ( error ) return false;
  JsonObject obj = _jsonDoc.as<JsonObject>();
  return buildScene(obj);
}

bool Racks::buildScene(JsonObject json) {
  Serial.println("> Building SCENE....\n");

  JsonObject remoteObject = json["driverRemotes"];
  _remoteRack->buildRemotes(remoteObject);

  JsonObject monitorObject = json["sensorMonitors"];
  _monitorRack->buildMonitors(monitorObject);

  Serial.println("> OK\n");

  return true;
}

