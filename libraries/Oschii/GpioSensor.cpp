#include "GpioSensor.h"

bool GpioSensor::getState() {
  return digitalRead(_pin);
}

bool GpioSensor::build(JsonObject json) {
  if ( !BinarySensor::build(json) ) return false;

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
    // not supported for I2C
    resistance = INPUT_PULLDOWN;

  } else {
    setError("Invalid resistor mode '" + _resistor + "'");
    return false;
  }

  if ( _pin == 0 && _resistor != "up" ) {
    setError("GPIO 0 is fixed to PULL-UP");
    return false;

  } else if ( _pin == 1 || _pin == 3 || ( _pin >= 6 && _pin <= 12 ) ) {
    setError("GPIOs 1, 3, 6-12 not recommended for use an inputs");
    return false;
  }

  pinMode(_pin, resistance);

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
