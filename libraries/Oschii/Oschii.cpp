#include "Oschii.h"

Oschii::Oschii() {
  _sensorRack = new SensorRack();
  _driverRack = new DriverRack();
  _remoteRack = new RemoteRack(_driverRack);
  _monitorRack = new MonitorRack(_sensorRack, _remoteRack);
}

bool Oschii::buildConfig(JsonObject json) {
  JsonArray sensorArray = json["sensors"];
  _sensorRack->buildSensors(sensorArray);

  JsonArray driverArray = json["drivers"];
  _driverRack->buildDrivers(driverArray);

  return true;
}

bool Oschii::buildScene(JsonObject json) {
  JsonObject remoteObject = json["driverRemotes"];
  _remoteRack->buildRemotes(remoteObject);

  JsonObject monitorObject = json["sensorMonitors"];
  _monitorRack->buildMonitors(monitorObject);

  return true;
}

void Oschii::loop() {
  _monitorRack->loop();
}