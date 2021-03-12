#include "Sensor.h"

Sensor::Sensor(I2CRack * i2cRack) {
  _i2cRack = i2cRack;
  _i2cModule = NULL;

  _built = false;
  _changed = false;
  _value = -1;

  _type = "uninitialized";
  _name = "uninitialized";
  _error = "";
}

bool Sensor::build(JsonObject json) {
  if ( _built ) {
    _error = "RuleTwoError: Sensor '" + _name + "' has already been built";
    return false;
  }

  if ( json.containsKey("name") ) {
    _name = json["name"].as<String>();
  } else {
    _error = "RuleTwoError: Sensor needs a name";
    return false;
  }

  if ( json.containsKey("type") ) {
    _type = json["type"].as<String>();
  } else {
    setError("No value given for 'type'");
    return false;
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
    Serial.println(moduleName);
  }

  _built = true;
  _lastChanged = millis();

  return true;
}

String Sensor::toPrettyJson() {
  String outputStr = "";
  serializeJsonPretty(toJson(), outputStr);
  return outputStr;
}

JsonObject Sensor::toJson() {
  _jsonRoot.clear();
  JsonObject json = _jsonRoot.to<JsonObject>();

  json["name"] = _name;
  json["type"] = _type;

  if ( _i2cModule != NULL ) {
    json["i2cModule"] = _i2cModule->getName();
  }

  return json;
}

String Sensor::toString() {
  return "(" + _name + ") " + _type + (_i2cModule==NULL ? "" : " i2c:"+_i2cModule->getName());
;
}

int Sensor::getValue() {
  return _value;
}

bool Sensor::hasChanged() {
  return _changed;
}

String Sensor::getType() {
  return _type;
}

String Sensor::getName() {
  return _name;
}

String Sensor::getError() {
  return _error;
}

void Sensor::setError(String error) {
  _error = "ERROR! Sensor '" + _name + "': " + error;
  Serial.println(_error);
}
