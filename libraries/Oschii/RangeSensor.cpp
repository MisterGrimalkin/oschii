#include "RangeSensor.h"

bool RangeSensor::build(JsonObject json) {
  if ( !Sensor::build(json) ) return false;

  _samples = 1;
  _sampleCount = 0;
  _interleave = false;

  _readingRange[MIN] = 0;
  _readingRange[MAX] = 3300;

  _valueRange[MIN] = 0;
  _valueRange[MAX] = 100;
  _flipRange = false;

  _bandPass[MIN] = 0;
  _bandPass[MAX] = 100;
  _bandCut[MIN] = 0;
  _bandCut[MAX] = 0;

  if ( json.containsKey("samples") )  _samples = json["samples"];
  if ( json.containsKey("interleave") )  _interleave = json["interleave"];

  if ( json.containsKey("readingRange") ) {
    JsonArray readingRange = json["readingRange"];
    if ( readingRange[MAX] < readingRange[MIN] ) {
      setError("Reading Range must be positive");
      return false;
    }
    _readingRange[MIN] = readingRange[MIN];
    _readingRange[MAX] = readingRange[MAX];
  }

  if ( json.containsKey("valueRange") ) {
    JsonArray valueRange = json["valueRange"];
    if ( valueRange[MAX] < valueRange[MIN] ) {
      setError("Value Range must be positive");
      return false;
    }
    _valueRange[MIN] = valueRange[MIN];
    _valueRange[MAX] = valueRange[MAX];
  }

  if ( json.containsKey("flipRange") )  _flipRange = json["flipRange"];

  if ( json.containsKey("bandPass") ) {
    JsonArray bandPass = json["bandPass"];
    if ( bandPass[MAX] < bandPass[MIN] ) {
      setError("Band Pass must be positive");
      return false;
    }
    _bandPass[MIN] = bandPass[MIN];
    _bandPass[MAX] = bandPass[MAX];
  } else {
    _bandPass[MIN] = _valueRange[MIN];
    _bandPass[MAX] = _valueRange[MAX];
  }

  if ( json.containsKey("bandCut") ) {
    JsonArray bandCut = json["bandCut"];
    if ( bandCut[MAX] < bandCut[MIN] ) {
      setError("Band Cut must be positive");
      return false;
    }
    _bandCut[MIN] = bandCut[MIN];
    _bandCut[MAX] = bandCut[MAX];
  }
  _bandCutActive = _bandCut[MAX] > _bandCut[MIN];


  if ( _samples > MAX_SAMPLES ) {
    setError("Max samples is " + String(MAX_SAMPLES));
    return false;
  }

  return true;
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

  _changed = (   tempValue != _value
              && tempValue >= _bandPass[MIN] && tempValue <= _bandPass[MAX]
              && !(_bandCutActive && tempValue >= _bandCut[MIN] && tempValue <= _bandCut[MAX])
            );
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
  double readingWindowSize = _readingRange[MAX] - _readingRange[MIN];
  double readingAmount = reading - _readingRange[MIN];

  double readingFraction = readingAmount / readingWindowSize;

  double valueWindowSize = _valueRange[MAX] - _valueRange[MIN];
  double valueAmount = readingFraction * valueWindowSize;

  if ( _flipRange ) {
    return (int)(_valueRange[MAX] - valueAmount);
  } else {
    return (int)(_valueRange[MIN] + valueAmount);
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

String RangeSensor::toString() {
  return Sensor::toString() + " samples:" + (_interleave ? "I:" : "B:") + String(_samples)
  + " reading[" + String(_readingRange[MIN]) + "-" + String(_readingRange[MAX])
  + "] value[" + String(_valueRange[MIN]) + "-" + String(_valueRange[MAX])
  + "] flip:" + String(_flipRange)
  + " bandPass[" + String(_bandPass[MIN]) + "-" + String(_bandPass[MAX])
  + "] bandCut[" + String(_bandCut[MIN]) + "-" + String(_bandCut[MAX])
  + String("]");
}

JsonObject RangeSensor::toJson() {
  JsonObject json = Sensor::toJson();
  json["samples"] = _samples;
  json["interleave"] = _interleave;

  JsonArray readingRange = json.createNestedArray("readingRange");
  readingRange.add(_readingRange[MIN]);
  readingRange.add(_readingRange[MAX]);

  JsonArray valueRange = json.createNestedArray("valueRange");
  valueRange.add(_valueRange[MIN]);
  valueRange.add(_valueRange[MAX]);

  json["flipRange"] = _flipRange;

  JsonArray bandPass = json.createNestedArray("bandPass");
  bandPass.add(_bandPass[MIN]);
  bandPass.add(_bandPass[MAX]);

  JsonArray bandCut = json.createNestedArray("bandCut");
  bandCut.add(_bandCut[MIN]);
  bandCut.add(_bandCut[MAX]);

  return json;
}
