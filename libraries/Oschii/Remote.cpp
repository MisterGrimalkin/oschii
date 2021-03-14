#include "Remote.h"

Remote::Remote(DriverRack * driverRack) {
  _driverRack = driverRack;
  _writeToIndex = 0;
  _transform = NULL;
}

Remote::~Remote() {
  if ( _transform != NULL ) delete _transform;
  for ( int i=0; i<_writeToIndex; i++ ) {
    RemoteWriteTo * writeTo = _writeTos[i];
    delete writeTo;
  }
}

void Remote::receive(int value) {
  int newValue = applyTransform(value);
  for ( int i=0; i<_writeToIndex; i++ ) {
    RemoteWriteTo * writeTo = _writeTos[i];
    writeTo->write(newValue);
  }
}

void Remote::update() {
  for ( int i=0; i<_writeToIndex; i++ ) {
    RemoteWriteTo * writeTo = _writeTos[i];
    writeTo->update();
  }
}

bool Remote::build(JsonObject json) {
  if ( json.containsKey("address") ) {
    _address = json["address"].as<String>();
  } else {
    setError("Must specify 'address'");
    return false;
  }

  if ( json.containsKey("valueTransform") ) {
    JsonObject transformJson = json["valueTransform"];
    _transform = new ValueTransform();
    if ( !_transform->build(transformJson) ) {
      setError(_transform->getError());
      return false;
    }
  }

  JsonArray writeToArray = json["writeTo"];

  for ( int i=0; i<writeToArray.size(); i++ ) {
    JsonObject writeToJson = writeToArray[i];

    RemoteWriteTo * writeTo = new RemoteWriteTo(_driverRack);
    if ( writeTo->build(writeToJson) ) {
      _writeTos[_writeToIndex++] = writeTo;
    } else {
      setError(writeTo->getError());
      return false;
    }
  }

  return true;
}

String Remote::getAddress() {
  return _address;
}

int Remote::applyTransform(int value) {
  if ( _transform == NULL ) {
    return value;
  } else {
    return _transform->apply(value);
  }
}

String Remote::toString() {
  String str = "<" + _address + "> ~~>";
  for ( int i=0; i<_writeToIndex; i++ ) {
    RemoteWriteTo * writeTo = _writeTos[i];
    str += " [" + writeTo->getDriverName() + "]";
  }
  return str;
}

String Remote::getError() {
  return _error;
}

void Remote::setError(String error) {
  _error = "Remote: " + error;
}

//JsonObject Remote::toJson() {
//  _jsonRoot.clear();
//  JsonObject json = _jsonRoot.to<JsonObject>();
//
//  json["address"] = _address;
//
//  JsonArray writeToArray = json.createNestedArray("writeTo");
//  for ( int i=0; i<_writeToIndex; i++ ) {
//    RemoteWriteTo * writeTo = _writeTos[i];
//    writeToArray.add(writeTo->toJson());
//  }
//
//  return json;
//}
