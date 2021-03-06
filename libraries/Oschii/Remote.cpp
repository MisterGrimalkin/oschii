#include "Remote.h"

Remote::Remote(DriverRack * driverRack) {
  _driverRack = driverRack;
  _writeToIndex = 0;
}

bool Remote::build(JsonObject json) {
  if ( json.containsKey("address") ) {
    _address = json["address"].as<String>();
  } else {
    return false;
  }

  JsonArray writeToArray = json["writeTo"];

  for ( int i=0; i<writeToArray.size(); i++ ) {
    JsonObject writeToJson = writeToArray[i];

    RemoteWriteTo * writeTo = new RemoteWriteTo(_driverRack);
    writeTo->build(writeToJson);
    _writeTos[_writeToIndex++] = writeTo;
  }

  return true;
}

void Remote::receive(int value) {
  for ( int i=0; i<_writeToIndex; i++ ) {
    RemoteWriteTo * writeTo = _writeTos[i];
    writeTo->write(value);
  }
}

String Remote::getAddress() {
  return _address;
}

String Remote::toString() {
  String str = "[" + _address + "] ~~>";
  for ( int i=0; i<_writeToIndex; i++ ) {
    RemoteWriteTo * writeTo = _writeTos[i];
    str += " (" + writeTo->getDriverName() + ")";
  }
  return str;
}

JsonObject Remote::toJson() {
  _jsonRoot.clear();
  JsonObject json = _jsonRoot.to<JsonObject>();

  json["address"] = _address;

  JsonArray writeToArray = json.createNestedArray("writeTo");
  for ( int i=0; i<_writeToIndex; i++ ) {
    RemoteWriteTo * writeTo = _writeTos[i];
    writeToArray.add(writeTo->toJson());
  }

  return json;
}
