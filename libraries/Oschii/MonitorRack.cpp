#include "MonitorRack.h"

MonitorRack::MonitorRack(SensorRack * sensorRack, RemoteRack * remoteRack) {
  _sensorRack = sensorRack;
  _remoteRack = remoteRack;
  _monitorIndex = 0;
}

String MonitorRack::buildMonitors(JsonObject json) {
  _monitorIndex = 0;

  Serial.println("== MONITORS ==");

  for (JsonPair kv : json) {
    String name = kv.key().c_str();
    JsonObject monitorJson = kv.value().as<JsonObject>();
    buildMonitor(name, monitorJson);
  }

  Serial.print("== Found: ");
  Serial.print(_monitorIndex);
  Serial.println(" ==\n");
  return "";
}


String MonitorRack::buildMonitor(String sensorName, JsonObject json) {
  Monitor * monitor = new Monitor(_sensorRack, _remoteRack);
  if ( monitor->build(sensorName, json) ) {
    Serial.println(" - " + monitor->toString());
    _monitors[_monitorIndex++] = monitor;
  }
  return "";
}

void MonitorRack::loop() {
  for ( int i=0; i<_monitorIndex; i++ ) {
    Monitor * monitor = _monitors[i];
    monitor->update();
  }
}

JsonObject MonitorRack::toJson() {
  _jsonRoot.clear();
  JsonObject json = _jsonRoot.createNestedObject("sensorMonitors");
  for ( int i=0; i<_monitorIndex; i++ ) {
    Monitor * monitor = _monitors[i];
    String key = monitor->getSensorName();
    json[key] = monitor->toJson();
  }
  return json;
}
