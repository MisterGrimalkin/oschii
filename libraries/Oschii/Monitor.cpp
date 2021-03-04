#include "Monitor.h"

Monitor::Monitor(SensorRack * sensorRack, RemoteRack * remoteRack) {
  _sensorRack = sensorRack;
  _remoteRack = remoteRack;

  _remoteIndex = 0;
}

bool Monitor::build(String sensorName, JsonObject json) {
  _sensor = _sensorRack->getSensor(sensorName);

  _onChange = true;
  _pollInterval = 0;
  _lastPolledAt = millis();

  if ( json.containsKey("onChange") )     _onChange     = json["onChange"];
  if ( json.containsKey("pollInterval") ) _pollInterval = json["pollInterval"];

  JsonArray fireRemotes = json["fireRemotes"];
  if ( fireRemotes.size() > MAX_MONITOR_REMOTES ) return false;

  for ( int i=0; i<fireRemotes.size(); i++ ) {
    Remote * remote = _remoteRack->getRemote(fireRemotes[i]);
    _remotes[_remoteIndex++] = remote;
  }

  return true;
}

void Monitor::update() {
  _sensor->readSensor();
  if (    ( _onChange && _sensor->hasChanged() )
       || ( millis() - _lastPolledAt >= _pollInterval)
  ) {
    for ( int i=0; i<_remoteIndex; i++ ) {
      Remote * remote = _remotes[i];
      remote->receive(_sensor->getValue());
    }
    _lastPolledAt = millis();
  }
}

String Monitor::toString() {
  String str = "(" + _sensor->getName() + ") -->";
  for ( int i=0; i<_remoteIndex; i++ ) {
    Remote * remote = _remotes[i];
    str += " (" + remote->getName() + ")";
  }
  return str;
}