#include "OSensor.h"

OSensor::OSensor() {
  OSensor(-1);
}

OSensor::OSensor(int index) {
  _index = index;
  _value = -1;
  _built = false;
  _changed = false;

  _type = "uninitialized";
  _name = "uninitialized";
  _error = "";
}

bool OSensor::build(JsonObject json) {
  if ( _built ) {
    _error = "WhatTheFuckError: Sensor " + String(_index) + " has already been built";
    return false;
  }

  if ( json.containsKey("type") ) {
    _type = json["type"].as<String>();
  } else {
    _error = "RuleTwoError: Sensor " + String(_index) + " needs a type";
    return false;
  }

  if ( json.containsKey("name") ) {
    _name = json["name"].as<String>();
  } else {
    _error = "RuleTwoError: Sensor " + String(_index) + " needs a name";
    return false;
  }

  _built = true;

  return true;
}

int OSensor::getValue() {
  return _value;
}

bool OSensor::hasChanged() {
  return _changed;
}

String OSensor::getType() {
  return _type;
}

String OSensor::getName() {
  return _name;
}

String OSensor::getError() {
  return _error;
}
