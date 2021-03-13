#include "RemoteRack.h"

RemoteRack::RemoteRack(DriverRack * driverRack) {
  _driverRack = driverRack;
  _remoteIndex = 0;
}

RemoteRack::~RemoteRack() {
  for ( int i=0;  i<_remoteIndex; i++ ) {
    Remote * remote = _remotes[i];
    delete remote;
  }
}

String RemoteRack::buildRemotes(JsonArray array) {
  _remoteIndex = 0;

  Serial.println("== REMOTES ==");

  String errorBuffer = "";

  for ( int i = 0; i < array.size(); i++ ) {
    JsonObject json = array[i];
    errorBuffer += buildRemote(json);
  }

  Serial.print("== Found: ");
  Serial.print(_remoteIndex);
  Serial.println(" ==\n");

  if ( errorBuffer != "" ) {
    Serial.println("Errors:");
    Serial.println(errorBuffer);
  }

  return "";
}

void RemoteRack::loop() {
  for ( int i=0;  i<_remoteIndex; i++ ) {
    Remote * remote = _remotes[i];
    remote->update();
  }
}

String RemoteRack::buildRemote(JsonObject json) {
  Remote * remote = new Remote(_driverRack);
  if ( remote->build(json) ) {
    Serial.println(" - " + remote->toString());
    _remotes[_remoteIndex++] = remote;
  } else {
    return remote->getError() + "\n";
  }
  return "";
}

Remote * RemoteRack::getRemote(String address) {
  for ( int i=0;  i<_remoteIndex; i++ ) {
    Remote * remote = _remotes[i];
    if ( remote->getAddress() == address ) {
      return remote;
    }
  }
  return NULL;
}

//JsonArray RemoteRack::toJson() {
//  _jsonRoot.clear();
//  JsonArray array= _jsonRoot.createNestedArray("driverRemotes");
//  for ( int i=0;  i<_remoteIndex; i++ ) {
//    Remote * remote = _remotes[i];
//    array.add(remote->toJson());
//  }
//  return array;
//}
