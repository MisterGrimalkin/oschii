#include "BinarySensor.h"

void BinarySensor::readSensor() {
  _changed = false;

  if ( millis() - _lastChanged < _bounceFilter ) return;

  bool state = getState();

  if ( _invert ) state = !state;

  int tempValue = state ? _onValue : _offValue;

  _changed = _value != tempValue && tempValue != -1;

  if ( _changed ) _lastChanged = millis();

  _value = tempValue;
}

bool BinarySensor::build(JsonObject json) {
  if ( !Sensor::build(json) ) {
    return false;
  }

  _onValue = 100;
  _offValue = -1;
  _invert = false;
  _bounceFilter = 0;

  if ( json.containsKey("onValue") )      _onValue      = json["onValue"];
  if ( json.containsKey("offValue") )     _offValue     = json["offValue"];
  if ( json.containsKey("invert") )       _invert       = json["invert"];
  if ( json.containsKey("bounceFilter") ) _bounceFilter = json["bounceFilter"];

  return true;
}

JsonObject BinarySensor::toJson() {
  JsonObject json = Sensor::toJson();

  json["onValue"]       = _onValue;
  json["offValue"]      = _offValue;
  json["invert"]        = _invert;
  json["bounceFilter"]  = _bounceFilter;

  return json;
}

String BinarySensor::toString() {
  return Sensor::toString()
          + " on:" +      String(_onValue)
          + " off:" +     String(_offValue)
          + " invert:" +  String(_invert)
          + " bounce:" +  String(_bounceFilter);
}
