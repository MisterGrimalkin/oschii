#include "PwmDriver.h"

void PwmDriver::fire(int value) {
}

bool PwmDriver::build(JsonObject json) {
  if ( !Driver::build(json) ) return false;

  _pin = -1;

  if ( json.containsKey("pin") ) {
    _pin = json["pin"];
  } else {
    setError("No value given for 'pin'");
    return false;
  }

  pinMode(_pin, OUTPUT);

  fire(_initialValue);

  return true;
}

JsonObject PwmDriver::toJson() {
  JsonObject json = Driver::toJson();

  json["pin"] = _pin;

  return json;
}

String PwmDriver::toString() {
  return Driver::toString()
          + " pin:" + String(_pin);
}
