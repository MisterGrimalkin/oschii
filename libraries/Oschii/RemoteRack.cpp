#include "RemoteRack.h"

RemoteRack::RemoteRack(DriverRack * driverRack) {
  _driverRack = driverRack;
  _remoteIndex = 0;
}

String RemoteRack::buildRemotes(JsonObject json) {
  _remoteIndex = 0;

  Serial.println("== REMOTES ==");

  for (JsonPair kv : json) {
    String name = kv.key().c_str();
    JsonObject remoteJson = kv.value().as<JsonObject>();
    buildRemote(name, remoteJson);
  }

  Serial.print("== Found: ");
  Serial.print(_remoteIndex);
  Serial.println(" ==\n");

  return "";
}

String RemoteRack::buildRemote(String name, JsonObject json) {
  Remote * remote = new Remote(_driverRack);
  if ( remote->build(name, json) ) {
    Serial.println(" - " + remote->toString());
    _remotes[_remoteIndex++] = remote;
  }
  return "";
}

Remote * RemoteRack::getRemote(String name) {
  for ( int i=0;  i<_remoteIndex; i++ ) {
    Remote * remote = _remotes[i];
    if ( remote->getName() == name ) {
      return remote;
    }
  }
  return NULL;
}

