#include "AnalogSensor.h"

bool AnalogSensor::build(JsonObject json) {
  if ( !RangeSensor::build(json) ) {
    return false;
  }

  _pin = -1;

  if ( json.containsKey("pin") ) {
    _pin = json["pin"];
  } else {
    _error = "RuleTwoError: Sensor '" + _name + "' needs value for 'pin'";
    return false;
  }

  pinMode(_pin, INPUT);

  return true;
}

int AnalogSensor::getReading() {
  int reading = analogRead(_pin);
  double milliVolts = (reading / 4095.0) * 3300;
  return (int)milliVolts;
}

String AnalogSensor::toString() {
  return RangeSensor::toString() + " pin:" + String(_pin);
}

JsonObject AnalogSensor::toJson() {
  JsonObject json = RangeSensor::toJson();
  json["pin"] = _pin;
  return json;
}
