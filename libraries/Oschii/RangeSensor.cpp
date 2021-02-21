#include "RangeSensor.h"

RangeSensor::RangeSensor() {
  RangeSensor(-1);
}

RangeSensor::RangeSensor(int index) {
  _index = index;

  _samples = 1;
  _interleave = false;
  _readingRange[MIN] = 0;
  _readingRange[MAX] = 3300;
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

bool RangeSensor::build(JsonObject json) {
  _error = "";

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

  if ( _samples > MAX_SAMPLES ) {
    _error = "Range Sensor Max samples is " + String(MAX_SAMPLES);
    return false;
  }

  return true;
}

bool RangeSensor::hasChanged() {
  return _changed;
}

int RangeSensor::getValue() {
  return _value;
}

String RangeSensor::getError() {
  return _error;
}

void RangeSensor::readSensor() {
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

int RangeSensor::capReading(int reading) {
  if ( reading < _readingRange[MIN] ) {
    return _readingRange[MIN];
  } else if ( reading > _readingRange[MAX] ) {
    return _readingRange[MAX];
  } else {
    return reading;
  }
}

int RangeSensor::mapToValue(int reading) {
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
