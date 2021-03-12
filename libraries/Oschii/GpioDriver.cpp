#include "GpioDriver.h"

void GpioDriver::fire(int value) {
  _value = value;

  if ( _value < 0 ) return;

  bool high =   (  _thresholdHighPass && _value >= _thresholdValue )
             || ( !_thresholdHighPass && _value <= _thresholdValue );

  if ( _invert ) high = !high;

  if ( _i2cGpioModule==NULL ) {
    digitalWrite(_pin, high);
  } else {
    _i2cGpioModule->write(_pin, high);
  }
}

bool GpioDriver::build(JsonObject json) {
  if ( json.containsKey("valueTransform") ) {
    setError("GPIO Drivers do not support Value Transforms");
    return false;
  }

  if ( !Driver::build(json) ) return false;

  _i2cGpioModule = NULL;
  _pin = -1;
  _thresholdValue = 1;
  _thresholdHighPass = true;

  if ( json.containsKey("pin") ) {
    _pin = json["pin"];
  } else {
    setError("No value given for 'pin'");
    return false;
  }

  if ( json.containsKey("thresholdValue") )    _thresholdValue    = json["thresholdValue"];
  if ( json.containsKey("thresholdHighPass") ) _thresholdHighPass = json["thresholdHighPass"];

  if ( _i2cModule == NULL ) {
    pinMode(_pin, OUTPUT);
  } else {
    _i2cGpioModule = (I2CGpioModule*)_i2cModule;
    _i2cGpioModule->setMode(_pin, true);
  }

  fire(_initialValue);

  return true;
}

JsonObject GpioDriver::toJson() {
  JsonObject json = Driver::toJson();

  json["pin"]               = _pin;
  json["thresholdValue"]    = _thresholdValue;
  json["thresholdHighPass"] = _thresholdHighPass;

  return json;
}

String GpioDriver::toString() {
  return Driver::toString()
          + " pin:" + String(_pin)
          + " threshold:" + String(_thresholdValue)
          + " highPass:" + String(_thresholdHighPass);
}