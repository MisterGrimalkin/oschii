#include "HCSRSensor.h"

bool HCSRSensor::build(JsonObject json) {
  if ( !RangeSensor::build(json) ) return false;

  if ( !json.containsKey("readingRange") ) {
    _readingRange[MIN] = 100;
    _readingRange[MAX] = 3500;
  }

  _trigPin = -1;
  _echoPin = -1;

  if ( json.containsKey("trigPin") )  _trigPin = json["trigPin"];
  if ( json.containsKey("echoPin") )  _echoPin = json["echoPin"];

  if ( _trigPin < 0 ) {
    _error = "RuleTwoError: Sensor " + String(_index) + " needs HCSR TRIG pin number";
    return false;
  }

  if ( _echoPin < 0 ) {
    _error = "RuleTwoError: Sensor " + String(_index) + " needs HCSR ECHO pin number";
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