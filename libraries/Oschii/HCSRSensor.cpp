#include "HCSRSensor.h"

HCSRSensor::HCSRSensor() {
  HCSRSensor(-1);
}

HCSRSensor::HCSRSensor(int index) {
  _index = index;

  _trigPin = -1;
  _echoPin = -1;
  _samples = 1;
  _interleave = false;
  _readingRange[MIN] = 200;
  _readingRange[MAX] = 2500;
  _valueRange[MIN] = 0;
  _valueRange[MAX] = 100;
  _flipRange = false;
  _triggerBand[MIN] = 0;
  _triggerBand[MAX] = 100;
  _sampleCount = 0;

  _error = "";

  _value = -1;
  _changed = false;
}

bool HCSRSensor::build(JsonObject json) {
  _error = "";

  if ( json.containsKey("trigPin") )  _trigPin = json["trigPin"];
  if ( json.containsKey("echoPin") )  _echoPin = json["echoPin"];
  if ( json.containsKey("samples") )  _samples = json["samples"];
  if ( json.containsKey("interleave") )  _interleave = json["interleave"];

  if ( json.containsKey("readingRange") ) {
    JsonArray readingRange = json["readingRange"];
    _readingRange[MIN] = readingRange[MIN];
    _readingRange[MAX] = readingRange[MAX];
  }

  if ( json.containsKey("valueRange") ) {
    JsonArray valueRange = json["valueRange"];
    _valueRange[MIN] = valueRange[MIN];
    _valueRange[MAX] = valueRange[MAX];
  }

  if ( json.containsKey("flipRange") )  _flipRange = json["flipRange"];

  if ( json.containsKey("triggerBand") ) {
    JsonArray triggerBand = json["triggerBand"];
    _triggerBand[MIN] = triggerBand[MIN];
    _triggerBand[MAX] = triggerBand[MAX];
  } else {
    _triggerBand[MIN] = _valueRange[MIN];
    _triggerBand[MAX] = _valueRange[MAX];
  }

  if ( _trigPin < 0 ) {
    _error = "HC-SR04 TRIG Pin not specified";
    return false;
  }

  if ( _echoPin < 0 ) {
    _error = "HC-SR04 ECHO Pin not specified";
    return false;
  }

  if ( _samples > MAX_SAMPLES ) {
    _error = "HC-SR04 Max samples is " + String(MAX_SAMPLES);
    return false;
  }

  pinMode(_trigPin, OUTPUT);
  pinMode(_echoPin, INPUT);

  return true;
}

void HCSRSensor::readSensor() {
  _changed = false;

  int tempValue = _value;
  if ( _samples > 1 ) {
    if ( _interleave ) {
      if ( _sampleCount < _samples ) {
        _sampleBuffer[_sampleCount++] = mapToValue(capReading(getReading()));
      } else {
        tempValue = getMedianValue(_sampleCount);
        _sampleCount = 0;
      }
    } else {
      for ( int i=0; i < _samples; i++ ) {
        _sampleBuffer[i] = mapToValue(capReading(getReading()));
      }
      tempValue = getMedianValue(_samples);
    }
  } else {
    tempValue = mapToValue(capReading(getReading()));
  }

  _changed = (tempValue != _value && tempValue >= _triggerBand[MIN] && tempValue <= _triggerBand[MAX]);
  _value = tempValue;
}

int compareIntegers (const void * a, const void * b) {
  return ( *(int*)a - *(int*)b );
}

int HCSRSensor::getMedianValue(int samples) {
  int values[samples];
  for ( int i=0; i<samples; i++ ) {
    values[i] = _sampleBuffer[i];
  }
  qsort(values, samples, sizeof(int), compareIntegers);
  return values[samples/2];
}

int HCSRSensor::mapToValue(int reading) {
  int readingWindowSize = _readingRange[MAX] - _readingRange[MIN];
  double readingPercentage = (reading - _readingRange[MIN]) / (double)readingWindowSize;
  int valueWindowSize = _valueRange[MAX] - _valueRange[MIN];
  double valueSize = readingPercentage * valueWindowSize;
  if ( _flipRange ) {
    return (int)(_valueRange[MAX] - valueSize);
  } else {
    return (int)(_valueRange[MIN] + valueSize);
  }
}

int HCSRSensor::capReading(int reading) {
  if ( reading < _readingRange[MIN] ) {
    return _readingRange[MIN];
  } else if ( reading > _readingRange[MAX] ) {
    return _readingRange[MAX];
  } else {
    return reading;
  }
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

bool HCSRSensor::hasChanged() {
  return _changed;
}

int HCSRSensor::getValue() {
  return _value;
}

String HCSRSensor::getError() {
  return _error;
}

void HCSRSensor::print() {
  Serial.println("   + Input:HC-SR04 ");
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
