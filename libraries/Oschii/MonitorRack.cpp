#include "MonitorRack.h"

MonitorRack::MonitorRack(SensorRack * sensorRack, RemoteRack * remoteRack, ReceiverRack * receiverRack) {
  _sensorRack = sensorRack;
  _remoteRack = remoteRack;
  _receiverRack = receiverRack;
  _monitorIndex = 0;
}

MonitorRack::~MonitorRack() {
  for ( int i=0; i<_monitorIndex; i++ ) {
    Monitor * monitor = _monitors[i];
    delete monitor;
  }
}

bool MonitorRack::buildMonitors(JsonArray array) {
  _monitorIndex = 0;

  Serial.println("== MONITORS ==");

  String errorBuffer = "";

  for ( int i = 0; i < array.size(); i++ ) {
    JsonObject json = array[i];
    errorBuffer += buildMonitor(json);
  }

  Serial.print("== Found: ");
  Serial.print(_monitorIndex);
  Serial.println(" ==\n");

  if ( errorBuffer != "" ) {
    Serial.println("Errors:");
    Serial.println(errorBuffer);
    return false;
  }

  return true;
}

String MonitorRack::buildMonitor(JsonObject json) {
  Monitor * monitor = new Monitor(_sensorRack, _remoteRack, _receiverRack);
  if ( monitor->build(json) ) {
    Serial.println(" - " + monitor->toString());
    _monitors[_monitorIndex++] = monitor;
  } else {
    return monitor->getError();
  }
  return "";
}

void MonitorRack::loop() {
  for ( int i=0; i<_monitorIndex; i++ ) {
    Monitor * monitor = _monitors[i];
    monitor->update();
  }
}

//JsonArray MonitorRack::toJson() {
//  _jsonRoot.clear();
//  JsonArray array= _jsonRoot.createNestedArray("sensorMonitors");
//  for ( int i=0; i<_monitorIndex; i++ ) {
//    Monitor * monitor = _monitors[i];
//    array.add(monitor->toJson());
//  }
//  return array;
//}
