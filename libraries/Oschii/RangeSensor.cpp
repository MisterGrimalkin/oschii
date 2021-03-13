#include "RangeSensor.h"

void RangeSensor::readSensor() {
  int tempValue = _value;

  _changed = false;

  if ( _samples > 1 ) {
    if ( _interleave ) {
      if ( _sampleCount < _samples ) {
        _sampleBuffer[_sampleCount++] = applyTransform(getReading());
      } else {
        tempValue = getMedianValue(_sampleCount);
        _sampleCount = 0;
      }
    } else {
      for ( int i=0; i < _samples; i++ ) {
        _sampleBuffer[i] = applyTransform(getReading());
      }
      tempValue = getMedianValue(_samples);
    }
  } else {
    tempValue = applyTransform(getReading());
  }

  _changed = (tempValue != _value);

  if ( _changed ) _lastChanged = millis();

  _value = tempValue;
}

int RangeSensor::applyTransform(int value) {
  if ( _transform == NULL ) {
    return value;
  } else {
    return _transform->apply(value);
  }
}

int compareIntegers (const void * a, const void * b) {
  return ( *(int*)a - *(int*)b );
}

int RangeSensor::getMedianValue(int samples) {
  int values[samples];
  for ( int i=0; i<samples; i++ ) {
    values[i] = _sampleBuffer[i];
  }
  qsort(values, samples, sizeof(int), compareIntegers);
  return values[samples/2];
}

bool RangeSensor::build(JsonObject json) {
  if ( !Sensor::build(json) ) return false;

  _value = -1;
  _samples = 1;
  _sampleCount = 0;
  _interleave = false;
  _transform = NULL;

  if ( json.containsKey("samples") )     _samples    = json["samples"];
  if ( json.containsKey("interleave") )  _interleave = json["interleave"];

  if ( _samples > MAX_SAMPLES ) {
    setError("Maximum samples is " + String(MAX_SAMPLES));
    return false;
  }

  if ( json.containsKey("valueTransform") ) {
    JsonObject transformJson = json["valueTransform"];
    _transform = new ValueTransform();
    if ( !_transform->build(transformJson) ) {
      setError(_transform->getError());
      return false;
    }
  }

  return true;
}

String RangeSensor::toString() {
  return Sensor::toString()
          + " samples:" + String(_samples)
          + " interleave:" + String(_interleave);
}

//JsonObject RangeSensor::toJson() {
//  JsonObject json = Sensor::toJson();
//
//  json["samples"] = _samples;
//  json["interleave"] = _interleave;
//
//  if ( _transform != NULL ) {
//    json["valueTransform"] = _transform->toJson();
//  }
//
//  return json;
//}
