#include "PwmDriver.h"

static int _nextChannel;

void PwmDriver::fire(int value) {
  _value = applyTransform(value);

  if ( value < 0 ) return;

  if ( _i2cPwmModule == NULL ) {
    ledcWrite(_channel, _value);
  } else {
    _i2cPwmModule->write(_pin, _value, _invert);
  }
}

bool PwmDriver::build(JsonObject json) {
  if ( !Driver::build(json) ) return false;

  _i2cPwmModule = NULL;
  _pin = -1;

  if ( json.containsKey("pin") ) {
    _pin = json["pin"];
  } else {
    setError("No value given for 'pin'");
    return false;
  }

  _channel = _nextChannel++;

  if ( _i2cModule == NULL ) {
    ledcAttachPin(_pin, _channel);
    ledcSetup(_channel, 4000, 8);
  } else {
    _i2cPwmModule = (I2CPwmModule*)_i2cModule;
  }

  fire(_initialValue);

  return true;
}

String PwmDriver::toString() {
  return Driver::toString()
          + " pin:" + String(_pin)
          + " channel:" + String(_channel);
}

//JsonObject PwmDriver::toJson() {
//  JsonObject json = Driver::toJson();
//
//  json["pin"] = _pin;
//
//  return json;
//}
