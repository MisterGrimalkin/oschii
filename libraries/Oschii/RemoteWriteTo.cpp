#include "RemoteWriteTo.h"

RemoteWriteTo::RemoteWriteTo(DriverRack * driverRack) {
  _driverRack = driverRack;
  _driver = NULL;
  _transform = NULL;
}

bool RemoteWriteTo::build(JsonObject json) {
  if ( json.containsKey("driver") ) {
    String driverName = json["driver"];
    _driver = _driverRack->getDriver(driverName);
  } else {
    return false;
  }

  if ( json.containsKey("valueTransform") ) {
    JsonObject transformJson = json["valueTransform"];
    _transform = new ValueTransform();
    _transform->build(transformJson);
  }

  return true;
}

void RemoteWriteTo::write(int value) {
  int writeValue = value;
  if ( _transform != NULL ) {
    writeValue = _transform->apply(value);
  }
  if ( _driver != NULL ) {
    _driver->fire(writeValue);
  }
}

String RemoteWriteTo::getDriverName() {
  return _driver->getName();
}

JsonObject RemoteWriteTo::toJson() {
  _jsonRoot.clear();
  JsonObject json = _jsonRoot.to<JsonObject>();

  if ( _driver != NULL ) {
    json["driver"] = _driver->getName();
  }

  if ( _transform != NULL ) {
    json["valueTransform"] = _transform->toJson();
  }

  return json;
}

