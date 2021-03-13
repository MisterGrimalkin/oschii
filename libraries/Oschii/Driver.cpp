#include "Driver.h"

Driver::Driver(I2CRack * i2cRack) {
  _i2cRack = i2cRack;
  _i2cModule = NULL;
  _transform = NULL;
  _built = false;
  _value = -1;

  _type = "uninitialized";
  _name = "uninitialized";
  _error = "";
}

Driver::~Driver() {
  if ( _transform != NULL ) delete _transform;
}

bool Driver::build(JsonObject json) {
  if ( _built ) {
    _error = "RuleTwoError: Driver '" + _name + "' has already been built";
    return false;
  }

  if ( json.containsKey("name") ) {
    _name = json["name"].as<String>();
  } else {
    _error = "RuleTwoError: Driver needs a name";
    return false;
  }

  if ( json.containsKey("type") ) {
    _type = json["type"].as<String>();
  } else {
    setError("No value given for 'type'");
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

  if ( json.containsKey("i2cModule") ) {
    String moduleName = json["i2cModule"].as<String>();
    _i2cModule = _i2cRack->getModule(moduleName);
    if ( _i2cModule==NULL ) {
      setError("No I2C module named '" + moduleName + "'");
      return false;
    }
    if ( _i2cModule->getType() != _type ) {
      setError("Wrong module type");
      return false;
    }
  }

  _initialValue = 0;
  _invert = false;

  if ( json.containsKey("initialValue") ) _initialValue = json["initialValue"];
  if ( json.containsKey("invert") )       _invert       = json["invert"];

  _value = _initialValue;
  _built = true;

  return true;
}

int Driver::applyTransform(int value) {
  if ( _transform == NULL ) {
    return value;
  } else {
    return _transform->apply(value);
  }
}

String Driver::toString() {
  return "[" + _name + "] " + _type
          + (_i2cModule==NULL ? "" : " {"+_i2cModule->getName() + "}")
          + " initial:" + String(_initialValue)
          + " invert:" + String(_invert);
}

int Driver::getValue() {
  return _value;
}

String Driver::getType() {
  return _type;
}

String Driver::getName() {
  return _name;
}

String Driver::getError() {
  return _error;
}

void Driver::setError(String error) {
  _error = "ERROR! Driver '" + _name + "': " + error;
  Serial.println(_error);
}

//String Driver::toPrettyJson() {
//  String outputStr = "";
//  serializeJsonPretty(toJson(), outputStr);
//  return outputStr;
//}
//
//JsonObject Driver::toJson() {
//  _jsonRoot.clear();
//  JsonObject json = _jsonRoot.to<JsonObject>();
//
//  json["name"]         = _name;
//  json["type"]         = _type;
//  json["initialValue"] = _initialValue;
//  json["invert"]       = _invert;
//
//  if ( _i2cModule != NULL ) {
//    json["i2cModule"] = _i2cModule->getName();
//  }
//
//  return json;
//}
