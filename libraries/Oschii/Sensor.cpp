#include "Sensor.h"

Sensor::Sensor() {
  Sensor(-1);
}

Sensor::Sensor(int index) {
  _index = index;
  _value = -1;
  _built = false;
  _changed = false;

  _type = "uninitialized";
  _name = "uninitialized";
  _error = "";
}

bool Sensor::build(JsonObject json) {
  if ( _built ) {
    _error = "RuleTwoError: Sensor '" + _name + "' has already been built";
    return false;
  }

  if ( json.containsKey("type") ) {
    _type = json["type"].as<String>();
  } else {
    _error = "RuleTwoError: Sensor '" + _name + "' needs a type";
    return false;
  }

  if ( json.containsKey("name") ) {
    _name = json["name"].as<String>();
  } else {
    _error = "RuleTwoError: Sensor needs a name";
    return false;
  }

  _built = true;

  return true;
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

String Sensor::toString() {
  return String(_index) + ":'" + _name + "' [" + _type + "]";
}

StaticJsonDocument<1024> doc;

JsonObject Sensor::toJson() {
  JsonObject json = doc.to<JsonObject>();
  json["name"] = _name;
  json["type"] = _type;
  return json;
}

String Sensor::toPrettyJson() {
  String outputStr = "";
  serializeJsonPretty(toJson(), outputStr);
  return outputStr;
}
