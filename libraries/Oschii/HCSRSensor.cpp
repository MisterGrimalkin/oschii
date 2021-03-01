#include "HCSRSensor.h"

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

bool HCSRSensor::build(JsonObject json) {
  if ( !RangeSensor::build(json) ) return false;

  if ( !json.containsKey("samples") ) {
    _samples = 9;
  }

  if ( !json.containsKey("interleave") ) {
    _interleave = true;
  }

  if ( !json.containsKey("readingRange") ) {
    _readingRange[MIN] = 300;
    _readingRange[MAX] = 5000;
  }

  if ( !json.containsKey("flipRange") ) {
    _flipRange = true;
  }

  _trigPin = -1;
  _echoPin = -1;

  if ( json.containsKey("trigPin") ) _trigPin = json["trigPin"];
  if ( json.containsKey("echoPin") ) _echoPin = json["echoPin"];

  if ( _trigPin < 0 ) {
    setError("No value given for 'trigPin'");
    return false;
  }

  if ( _echoPin < 0 ) {
    setError("No value given for 'echoPin'");
    return false;
  }

  pinMode(_trigPin, OUTPUT);
  pinMode(_echoPin, INPUT);

  return true;
}

JsonObject HCSRSensor::toJson() {
  JsonObject json = RangeSensor::toJson();
  json["trigPin"] = _trigPin;
  json["echoPin"] = _echoPin;
  return json;
}

String HCSRSensor::toString() {
  return RangeSensor::toString() + " trigPin:" + String(_trigPin) + " echoPin:" + String(_echoPin);
}
