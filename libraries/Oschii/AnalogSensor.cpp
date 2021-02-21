#include "AnalogSensor.h"

bool AnalogSensor::build(JsonObject json) {
  if ( !RangeSensor::build(json) ) {
    return false;
  }

  _pin = -1;

  if ( json.containsKey("pin") ) _pin = json["pin"];

  if ( _pin < 0 ) {
    _error = "Analog Pin not specified";
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

void AnalogSensor::print() {
  Serial.print  ("   + Input-");
  Serial.print  (_index);
  Serial.println(":Analog");
  Serial.print  ("     [pin:");
  Serial.print(_pin);
  Serial.print(" samples:");
  Serial.print(_interleave ? "~" : "#");
  Serial.print(_samples);
  Serial.print(" reading(");
  Serial.print(_readingRange[MIN]);
  Serial.print("-");
  Serial.print(_readingRange[MAX]);
  Serial.print(") value(");
  Serial.print(_flipRange ? _valueRange[MAX] : _valueRange[MIN]);
  Serial.print("-");
  Serial.print(_flipRange ? _valueRange[MIN] : _valueRange[MAX]);
  Serial.print(") trigger(");
  Serial.print(_triggerBand[MIN]);
  Serial.print("-");
  Serial.print(_triggerBand[MAX]);
  Serial.println(")]");
}
