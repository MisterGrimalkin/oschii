#include "PwmDriver.h"

static int _nextChannel;

void PwmDriver::fire(int value) {
  _value = value;

  if ( value < 0 ) return;

  ledcWrite(_channel, value);
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

  _channel = _nextChannel++;

  ledcAttachPin(_pin, _channel);
  ledcSetup(_channel, 4000, 8);

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
          + " pin:" + String(_pin)
          + " channel:" + String(_channel);
}
