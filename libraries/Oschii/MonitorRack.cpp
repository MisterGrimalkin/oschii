#include "MonitorRack.h"

MonitorRack::MonitorRack(SensorRack * sensorRack, RemoteRack * remoteRack) {
  _sensorRack = sensorRack;
  _remoteRack = remoteRack;
  _monitorIndex = 0;
}

String MonitorRack::buildMonitors(JsonArray array) {
  _monitorIndex = 0;

  Serial.println("== MONITORS ==");

  for ( int i = 0; i < array.size(); i++ ) {
    JsonObject json = array[i];
    buildMonitor(json);
  }

  Serial.print("== Found: ");
  Serial.print(_monitorIndex);
  Serial.println(" ==\n");
  return "";
}


String MonitorRack::buildMonitor(JsonObject json) {
  Monitor * monitor = new Monitor(_sensorRack, _remoteRack);
  if ( monitor->build(json) ) {
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

JsonArray MonitorRack::toJson() {
  _jsonRoot.clear();
  JsonArray array= _jsonRoot.createNestedArray("sensorMonitors");
  for ( int i=0; i<_monitorIndex; i++ ) {
    Monitor * monitor = _monitors[i];
    array.add(monitor->toJson());
  }
  return array;
}
