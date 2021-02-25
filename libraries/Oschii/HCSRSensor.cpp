#include "HCSRSensor.h"

bool HCSRSensor::build(JsonObject json) {
  if ( !RangeSensor::build(json) ) return false;

  if ( !json.containsKey("readingRange") ) {
    _readingRange[MIN] = 100;
    _readingRange[MAX] = 3500;
  }

  _trigPin = -1;
  _echoPin = -1;

  if ( json.containsKey("trigPin") ) {
    _trigPin = json["trigPin"];
  } else {
    _error = "RuleTwoError: Sensor '" + _name + "' needs value for 'trigPin'";
    return false;
  }

  if ( json.containsKey("echoPin") ) {
    _echoPin = json["echoPin"];
  } else {
    _error = "RuleTwoError: Sensor '" + _name + "' needs value for 'echoPin'";
    return false;
  }

  pinMode(_trigPin, OUTPUT);
  pinMode(_echoPin, INPUT);

  return true;
}

int HCSRSensor::getReading() {
  digitalWrite(_echoPin, LOW);
  digitalWrite(_trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(_trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(_trigPin, LOW);
  int reading = pulseIn(_echoPin, HIGH, _readingRange[MAX]);
  if ( reading == 0 ) {
    reading = _readingRange[MAX];
  }
  return reading;
}

String HCSRSensor::toString() {
  return RangeSensor::toString() + " trigPin:" + String(_trigPin) + " echoPin:" + String(_echoPin);
}

JsonObject HCSRSensor::toJson() {
  JsonObject json = RangeSensor::toJson();
  json["trigPin"] = _trigPin;
  json["echoPin"] = _echoPin;
  return json;
}