#include "Sensor.h"

Sensor::Sensor() {
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
  JsonObject json = _jsonRoot.to<JsonObject>();

  json["name"] = _name;
  json["type"] = _type;

  return json;
}

String Sensor::toString() {
  return "[" + _name + "] " + _type;
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
}
