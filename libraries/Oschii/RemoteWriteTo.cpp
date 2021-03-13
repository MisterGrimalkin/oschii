#include "RemoteWriteTo.h"

RemoteWriteTo::RemoteWriteTo(DriverRack * driverRack) {
  _driverRack = driverRack;
  _driver = NULL;
  _transform = NULL;
  _envelope = NULL;
}

bool RemoteWriteTo::build(JsonObject json) {
  if ( json.containsKey("driver") ) {
    String driverName = json["driver"];
    _driver = _driverRack->getDriver(driverName);
  } else {
    setError("Must specify 'Driver'");
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

  if ( json.containsKey("envelope") ) {
    JsonArray envelopeJson = json["envelope"];
    _envelope = new Envelope();
    if ( !_envelope->build(envelopeJson) ) {
      setError(_envelope->getError());
      return false;
    }
  }

  return true;
}

void RemoteWriteTo::write(int value) {
  write(value, false);
}

void RemoteWriteTo::write(int value, bool fromEnvelope) {
  int writeValue = value;
  if ( _transform != NULL ) {
    writeValue = _transform->apply(value);
  }
  bool skipDriver = false;
  if ( !fromEnvelope && _envelope != NULL ) {
    if ( writeValue==0 ) {
      _envelope->stop();
    } else if ( writeValue > 0 ) {
      skipDriver = true;
      _envelope->start(writeValue);
    }
  }
  if ( !skipDriver && _driver != NULL ) {
    _driver->fire(writeValue);
  }
}

void RemoteWriteTo::update() {
  if ( _envelope != NULL && _envelope->isRunning() ) {
    write(_envelope->get(), true);
  }
}

String RemoteWriteTo::getDriverName() {
  return _driver->getName();
}

String RemoteWriteTo::getError() {
  return _error;
}

void RemoteWriteTo::setError(String error) {
  _error = "Remote Write To: " + error;
}

//JsonObject RemoteWriteTo::toJson() {
//  _jsonRoot.clear();
//  JsonObject json = _jsonRoot.to<JsonObject>();
//
//  if ( _driver != NULL ) {
//    json["driver"] = _driver->getName();
//  }
//
//  if ( _transform != NULL ) {
//    json["valueTransform"] = _transform->toJson();
//  }
//
//  if ( _envelope != NULL ) {
//    json["envelope"] = _envelope->toJson();
//  }
//
//  return json;
//}
