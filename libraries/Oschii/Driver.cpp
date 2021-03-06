#include "Driver.h"

Driver::Driver() {
  _built = false;
  _value = -1;

  _type = "uninitialized";
  _name = "uninitialized";
  _error = "";
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

  _initialValue = -1;

  if ( json.containsKey("initialValue") ) _initialValue = json["initialValue"];

  _value = _initialValue;
  _built = true;

  return true;
}

String Driver::toPrettyJson() {
  String outputStr = "";
  serializeJsonPretty(toJson(), outputStr);
  return outputStr;
}

JsonObject Driver::toJson() {
  _jsonRoot.clear();
  JsonObject json = _jsonRoot.to<JsonObject>();

  json["name"]         = _name;
  json["type"]         = _type;
  json["initialValue"] = _initialValue;

  return json;
}

String Driver::toString() {
  return "[" + _name + "] " + _type
          + " initial:" + String(_initialValue);
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
}
