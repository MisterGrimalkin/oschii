#include "ValueTransform.h"

int ValueTransform::apply(int value) {
  return mapToOutput(capInput(value));
}

int ValueTransform::capInput(int input) {
  int newInput = input;

  if ( input < _inputRange[MIN] ) {
    if ( _discardOutliers == -1 ) {
      newInput = _lastInput;
    } else if ( _outlierCount >= _discardOutliers ) {
      newInput = _inputRange[MIN];
      _outlierCount = 0;
    } else {
      newInput = _lastInput;
      _outlierCount++;
    }

  } else if ( input > _inputRange[MAX] ) {
    if ( _discardOutliers == -1 ) {
      newInput = _lastInput;
    } else if ( _outlierCount >= _discardOutliers ) {
      newInput = _inputRange[MAX];
      _outlierCount = 0;
    } else {
      newInput = _lastInput;
      _outlierCount++;
    }
  }

  return _lastInput = newInput;
}

int ValueTransform::mapToOutput(int input) {
  double inputWindowSize = _inputRange[MAX] - _inputRange[MIN];
  double inputAmount = input - _inputRange[MIN];

  double inputFraction = inputAmount / inputWindowSize;

  double outputWindowSize = _outputRange[MAX] - _outputRange[MIN];
  double outputAmount = inputFraction * outputWindowSize;

  if ( _flipRange ) {
    return (int)(_outputRange[MAX] - outputAmount);
  } else {
    return (int)(_outputRange[MIN] + outputAmount);
  }
}

void ValueTransform::setInputRange(int min, int max) {
  setInputRange(min, max, -1);
}

void ValueTransform::setInputRange(int min, int max, int discardOutliers) {
  _inputRange[MIN] = min;
  _inputRange[MAX] = max;
  _discardOutliers = discardOutliers;
}

void ValueTransform::setOutputRange(int min, int max) {
  setOutputRange(min, max, false);
}

void ValueTransform::setOutputRange(int min, int max, bool flipRange) {
  _outputRange[MIN] = min;
  _outputRange[MAX] = max;
  _flipRange = flipRange;
}

void ValueTransform::setBandPass(int min, int max) {
  _bandPass[MIN] = min;
  _bandPass[MAX] = max;
}

void ValueTransform::setBandCut(int min, int max) {
  _bandCut[MIN] = min;
  _bandCut[MAX] = max;
}

bool ValueTransform::build(JsonObject json) {
  _inputRange[MIN] = 0;
  _inputRange[MAX] = 4095;
  _discardOutliers = 0;
  _outlierCount = 0;

  _outputRange[MIN] = 0;
  _outputRange[MAX] = 100;
  _flipRange = false;

  _bandPass[MIN] = 0;
  _bandPass[MAX] = 100;
  _bandCut[MIN] = -1;
  _bandCut[MAX] = -1;

  if ( json.containsKey("inputRange") ) {
    JsonArray inputRange = json["inputRange"];
    if ( inputRange[MAX] < inputRange[MIN] ) {
      Serial.println("Input range must be positive");
      return false;
    }
    _inputRange[MIN] = inputRange[MIN];
    _inputRange[MAX] = inputRange[MAX];
  }

  if ( json.containsKey("discardOutliers") ) _discardOutliers = json["discardOutliers"];

  if ( json.containsKey("outputRange") ) {
    JsonArray outputRange = json["outputRange"];
    if ( outputRange[MAX] < outputRange[MIN] ) {
      Serial.println("Output range must be positive");
      return false;
    }
    _outputRange[MIN] = outputRange[MIN];
    _outputRange[MAX] = outputRange[MAX];
  }

  if ( json.containsKey("flipRange") )  _flipRange = json["flipRange"];

  if ( json.containsKey("bandPass") ) {
    JsonArray bandPass = json["bandPass"];
    if ( bandPass[MAX] < bandPass[MIN] ) {
      Serial.println("Band Pass must be positive");
      return false;
    }
    _bandPass[MIN] = bandPass[MIN];
    _bandPass[MAX] = bandPass[MAX];
  } else {
    _bandPass[MIN] = _outputRange[MIN];
    _bandPass[MAX] = _outputRange[MAX];
  }

  if ( json.containsKey("bandCut") ) {
    JsonArray bandCut = json["bandCut"];
    if ( bandCut[MAX] < bandCut[MIN] ) {
      Serial.println("Band Cut must be positive");
      return false;
    }
    _bandCut[MIN] = bandCut[MIN];
    _bandCut[MAX] = bandCut[MAX];
  }

  return false;
}

JsonObject ValueTransform::toJson() {
  _jsonRoot.clear();
  JsonObject json = _jsonRoot.to<JsonObject>();

  JsonArray inputRange = json.createNestedArray("inputRange");
  inputRange.add(_inputRange[MIN]);
  inputRange.add(_inputRange[MAX]);

  json["discardOutliers"] = _discardOutliers;

  JsonArray outputRange = json.createNestedArray("outputRange");
  outputRange.add(_outputRange[MIN]);
  outputRange.add(_outputRange[MAX]);

  json["flipRange"] = _flipRange;

  JsonArray bandPass = json.createNestedArray("bandPass");
  bandPass.add(_bandPass[MIN]);
  bandPass.add(_bandPass[MAX]);

  JsonArray bandCut = json.createNestedArray("bandCut");
  bandCut.add(_bandCut[MIN]);
  bandCut.add(_bandCut[MAX]);
  
  return json;
}
