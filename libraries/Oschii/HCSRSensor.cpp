#include "HCSRSensor.h"

bool HCSRSensor::build(JsonObject json) {
  _readingRange[MIN] = 100;
  _readingRange[MAX] = 3500;

  if ( !RangeSensor::build(json) ) {
    return false;
  }

  _trigPin = -1;
  _echoPin = -1;

  if ( json.containsKey("trigPin") )  _trigPin = json["trigPin"];
  if ( json.containsKey("echoPin") )  _echoPin = json["echoPin"];

  if ( _trigPin < 0 ) {
    _error = "HC-SR04 TRIG Pin not specified";
    return false;
  }

  if ( _echoPin < 0 ) {
    _error = "HC-SR04 ECHO Pin not specified";
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

void HCSRSensor::print() {
  Serial.print  ("   + Input-");
  Serial.print  (_index);
  Serial.println(":HC-SR04 ");
  Serial.print  ("     [trig:");
  Serial.print(_trigPin);
  Serial.print(" echo:");
  Serial.print(_echoPin);
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
