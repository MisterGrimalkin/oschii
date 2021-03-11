#include "GpioSensor.h"

bool GpioSensor::getState() {
  bool value = false;
  if ( _i2cGpioModule==NULL ) {
    value = digitalRead(_pin);
  } else {
    value = _i2cGpioModule->read(_pin);
  }
  return value;
}

bool GpioSensor::build(JsonObject json) {
  if ( !BinarySensor::build(json) ) return false;

  _i2cGpioModule = NULL;
  _pin = -1;
  _resistor = "off";

  if ( json.containsKey("pin") )       _pin       = json["pin"];
  if ( json.containsKey("resistor") )  _resistor  = json["resistor"].as<String>();

  if ( _pin < 0 ) {
    setError("No value for 'pin'");
    return false;
  }

  int resistance;

  if ( _resistor == "off" ) {
    resistance = INPUT;

  } else if ( _resistor == "up" ) {
    resistance = INPUT_PULLUP;

  } else if ( _resistor == "down" ) {
    if ( _i2cModule == NULL ) {
      resistance = INPUT_PULLDOWN;
    } else {
      setError("Pull-down resistor not available with I2C");
      return false;
    }

  } else {
    setError("Invalid resistor mode '" + _resistor + "'");
    return false;
  }

  if ( _i2cModule == NULL ) {
    if ( _pin == 0 && _resistor != "up" ) {
      setError("GPIO 0 is fixed to PULL-UP");
      return false;

    } else if ( _pin == 1 || _pin == 3 || ( _pin >= 6 && _pin <= 12 ) ) {
      setError("GPIOs 1, 3, 6-12 not recommended for use an inputs");
      return false;
    }

    pinMode(_pin, resistance);

  } else {
    _i2cGpioModule = (I2CGpioModule*)_i2cModule;

    if ( _pin > 15 ) {
      setError("I2C GPIO 'pin' must be 0-15");
    }

    _i2cGpioModule->setMode(_pin, false);
    _i2cGpioModule->setResistance(_pin, resistance==INPUT_PULLUP);
  }

  return true;
}

JsonObject GpioSensor::toJson() {
  JsonObject json = BinarySensor::toJson();

  json["pin"]       = _pin;
  json["resistor"]  = _resistor;

  return json;
}

String GpioSensor::toString() {
  return BinarySensor::toString()
          + " pin:" + String(_pin)
          + " resistor:" + _resistor;
}
