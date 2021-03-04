#include "GpioDriver.h"

void GpioDriver::fire(int value) {
  _value = value;

  if ( _value < 0 ) return;

  bool high =   (  _thresholdHighPass && _value >= _thresholdValue )
             || ( !_thresholdHighPass && _value <= _thresholdValue );

  if ( _invert ) high = !high;

  digitalWrite(_pin, high);
}

bool GpioDriver::build(JsonObject json) {
  if ( !Driver::build(json) ) return false;

  _pin = -1;
  _thresholdValue = 1;
  _thresholdHighPass = true;
  _invert = false;

  if ( json.containsKey("pin") ) {
    _pin = json["pin"];
  } else {
    setError("No value given for 'pin'");
    return false;
  }

  if ( json.containsKey("thresholdValue") )    _thresholdValue    = json["thresholdValue"];
  if ( json.containsKey("thresholdHighPass") ) _thresholdHighPass = json["thresholdHighPass"];
  if ( json.containsKey("invert") )            _invert            = json["invert"];

  pinMode(_pin, OUTPUT);

  fire(_initialValue);

  return true;
}

JsonObject GpioDriver::toJson() {
  JsonObject json = Driver::toJson();

  json["pin"]               = _pin;
  json["thresholdValue"]    = _thresholdValue;
  json["thresholdHighPass"] = _thresholdHighPass;
  json["invert"]            = _invert;

  return json;
}

String GpioDriver::toString() {
  return Driver::toString()
          + " pin:" + String(_pin)
          + " threshold:" + String(_thresholdValue)
          + " highPass:" + String(_thresholdHighPass)
          + " invert:" + String(_invert);
}