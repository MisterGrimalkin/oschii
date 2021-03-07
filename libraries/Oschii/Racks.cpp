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
  _monitorRack->loop();
  _remoteRack->loop();
}

bool Racks::buildConfig(String jsonString) {
  const char * json = jsonString.c_str();
  DeserializationError error = deserializeJson(_jsonDoc, json);
  if ( error ) return false;
  JsonObject obj = _jsonDoc.as<JsonObject>();
  return buildConfig(obj);
}

bool Racks::buildConfig(JsonObject json) {
  int freeHeapSize = esp_get_free_heap_size();

  Serial.println("> Building CONFIGURATION....\n");

  JsonArray sensorArray = json["sensors"];
  JsonArray driverArray = json["drivers"];
  JsonArray remoteArray = json["driverRemotes"];
  JsonArray monitorArray = json["sensorMonitors"];

  _sensorRack->buildSensors(sensorArray);
  _driverRack->buildDrivers(driverArray);
  _remoteRack->buildRemotes(remoteArray);
  _monitorRack->buildMonitors(monitorArray);

  Serial.print("Configuration used ");
  Serial.print(freeHeapSize - esp_get_free_heap_size());
  Serial.print(" bytes (free heap size: ");
  Serial.print(esp_get_free_heap_size());
  Serial.println(" bytes)\n");

  Serial.println("> OK\n");

  return true;
}

JsonObject Racks::toJson() {
  _jsonDoc.clear();
  JsonObject json = _jsonDoc.createNestedObject("config");
  json["sensors"] = _sensorRack->toJson();
  json["drivers"] = _driverRack->toJson();
  json["driverRemotes"] = _remoteRack->toJson();
  json["sensorMonitors"] = _monitorRack->toJson();
  return json;
}

String Racks::toPrettyJson() {
  String outputStr = "";
  serializeJsonPretty(toJson(), outputStr);
  return outputStr;
}
