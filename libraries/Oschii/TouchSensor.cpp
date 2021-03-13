#include "TouchSensor.h"

#define ECHO_RAW_READING false

bool TouchSensor::getState() {
  int reading = touchRead(_pin);
  if ( ECHO_RAW_READING ) {
    Serial.print("~=");
    Serial.println(reading);
  }
  if ( _triggerHighPass ) {
    return reading >= _triggerThreshold;
  } else {
    return reading <= _triggerThreshold;
  }
}

bool TouchSensor::build(JsonObject json) {
  if ( !BinarySensor::build(json) ) return false;

  _pin = -1;
  _triggerThreshold = 30;
  _triggerHighPass = false;

  if ( json.containsKey("pin") )              _pin              = json["pin"];
  if ( json.containsKey("triggerThreshold") ) _triggerThreshold = json["triggerThreshold"];
  if ( json.containsKey("triggerHighPass") )  _triggerHighPass  = json["triggerHighPass"];

  if ( _pin < 0 ) {
    setError("No value for 'pin'");
    return false;
  }

  if ( _triggerThreshold < 0 || _triggerThreshold > 100 ) {
    setError("'triggerThreshold' must be between 0 and 100");
    return false;
  }
}

String TouchSensor::toString() {
  return BinarySensor::toString()
          + " pin:" + String(_pin)
          + " threshold:" + String(_triggerThreshold)
          + " highPass:" + String(_triggerHighPass);
}

//JsonObject TouchSensor::toJson() {
//  JsonObject json = BinarySensor::toJson();
//
//  json["pin"]              = _pin;
//  json["triggerThreshold"] = _triggerThreshold;
//  json["triggerHighPass"]  = _triggerHighPass;
//
//  return json;
//}
