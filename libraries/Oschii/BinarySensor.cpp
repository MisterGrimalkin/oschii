#include "BinarySensor.h"

void BinarySensor::readSensor() {
  _changed = false;

  if ( millis() - _lastChanged < _bounceFilter ) return;

  bool state = getState();

  if ( _invert ) state = !state;

  if ( _holdOnFilter > 0 ) {
    if ( state && !_lastState ) {
      if ( _holdStarted > 0 ) {
        if ( millis() - _holdStarted >= _holdOnFilter ) {
          _holdStarted = -1;
        } else {
          state = _lastState;
        }
      } else {
        _holdStarted = millis();
        state = _lastState;
      }
    } else {
      _holdStarted = -1;
    }
  }

  _lastState = state;

  int tempValue = state ? _onValue : _offValue;

  _changed = _value != tempValue && tempValue != -1;

  if ( _changed ) _lastChanged = millis();

  _value = tempValue;
}

bool BinarySensor::build(JsonObject json) {
  if ( !Sensor::build(json) ) return false;

  _holding = false;
  _holdStarted = -1;

  _onValue = 100;
  _offValue = -1;
  _invert = false;
  _bounceFilter = 0;
  _holdOnFilter = 0;

  if ( json.containsKey("onValue") )      _onValue      = json["onValue"];
  if ( json.containsKey("offValue") )     _offValue     = json["offValue"];
  if ( json.containsKey("invert") )       _invert       = json["invert"];
  if ( json.containsKey("bounceFilter") ) _bounceFilter = json["bounceFilter"];
  if ( json.containsKey("holdOnFilter") ) _holdOnFilter = json["holdOnFilter"];

  if ( json.containsKey("valueTransform") ) {
    setError("Binary Sensors do not support Value Transforms");
    return false;
  }

  return true;
}

String BinarySensor::toString() {
  return Sensor::toString()
          + " on:" +      String(_onValue)
          + " off:" +     String(_offValue)
          + " invert:" +  String(_invert)
          + " bounce:" +  String(_bounceFilter)
          + " hold:" +  String(_holdOnFilter);
}

//JsonObject BinarySensor::toJson() {
//  JsonObject json = Sensor::toJson();
//
//  json["onValue"]       = _onValue;
//  json["offValue"]      = _offValue;
//  json["invert"]        = _invert;
//  json["bounceFilter"]  = _bounceFilter;
//  json["holdOnFilter"]  = _holdOnFilter;
//
//  return json;
//}
