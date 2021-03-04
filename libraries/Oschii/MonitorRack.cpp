#include "MonitorRack.h"

MonitorRack::MonitorRack(SensorRack * sensorRack, RemoteRack * remoteRack) {
  _sensorRack = sensorRack;
  _remoteRack = remoteRack;
  _monitorIndex = 0;
}

String MonitorRack::buildMonitors(JsonObject json) {
  Serial.println("== Building Monitors ==");
  for (JsonPair kv : json) {
    String name = kv.key().c_str();
    JsonObject monitorJson = kv.value().as<JsonObject>();
    buildMonitor(name, monitorJson);
  }
  Serial.print("== Found: ");
  Serial.println(_monitorIndex);
  Serial.println(" ==\n");
  return "";
}

String MonitorRack::buildMonitor(String sensorName, JsonObject json) {
  Monitor * monitor = new Monitor(_sensorRack, _remoteRack);
  if ( monitor->build(sensorName, json) ) {
    Serial.println(" " + monitor->toString());
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
