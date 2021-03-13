#include "ValueTransform.h"

ValueTransform::~ValueTransform() {
}

int ValueTransform::apply(int value) {
  return mapToOutput(capInput(value));
}

int ValueTransform::capInput(int input) {
  int newInput = input;

  if ( input < _inputRange[MIN] || input > _inputRange[MAX] ) {
    // Outlier

    if ( _discardAllOutliers ) {
      // Discard ALL
      newInput = _lastInput;

    } else if ( _discardOutliers > 0 ) {
      if ( _outlierCount < _discardOutliers ) {
        // Discard this outlier
        newInput = _lastInput;
        _outlierCount++;
      } else {
        // Discarded enough outliers, cap to Input Range
        if ( newInput < _inputRange[MIN] ) {
          newInput = _inputRange[MIN];
        } else {
          newInput = _inputRange[MAX];
        }
        _outlierCount = 0;
      }

    } else if ( _setOutliersTo >= 0 ) {
      // Override outlier value
      newInput = _setOutliersTo;

    } else {
      // Cap to range
      if ( newInput < _inputRange[MIN] ) {
        newInput = _inputRange[MIN];
      } else {
        newInput = _inputRange[MAX];
      }
    }

  } else {
    // Within Range
    _outlierCount = 0;
   }

  return _lastInput = newInput;
}

int ValueTransform::mapToOutput(int input) {
  double outputWindowSize = _outputRange[MAX] - _outputRange[MIN];
  double inputAmount = _flipRange ? _inputRange[MAX] - input : input - _inputRange[MIN];

  int outputValue = 0;
  double inputWindowSize = (_inputRange[MAX] - _inputRange[MIN]);
  double inputFraction = inputAmount / inputWindowSize;
  if ( _mirrorRange && inputFraction > 0.5 ) {
    inputFraction = 1 - inputFraction;
  }
  double outputAmount = inputFraction * outputWindowSize;
  outputValue = (int)(_outputRange[MIN] + outputAmount);

  return outputValue;
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
  _error = "";

  _inputRange[MIN] = 0;
  _inputRange[MAX] = 100;

  _discardAllOutliers = false;
  _discardOutliers = -1;
  _outlierCount = 0;
  _setOutliersTo = -1;

  _outputRange[MIN] = 0;
  _outputRange[MID] = -1;
  _outputRange[MAX] = 100;
  _flipRange = false;
  _mirrorRange = false;

  _bandPass[MIN] = 0;
  _bandPass[MAX] = 100;
  _bandCut[MIN] = -1;
  _bandCut[MAX] = -1;

  if ( json.containsKey("inputRange") ) {
    JsonArray inputRange = json["inputRange"];
    if ( inputRange[MAX] < inputRange[MIN] ) {
      setError("Input range must be positive");
      return false;
    }
    _inputRange[MIN] = inputRange[MIN];
    _inputRange[MAX] = inputRange[MAX];
  }

  if ( json.containsKey("discardAllOutliers") ) _discardAllOutliers = json["discardAllOutliers"];
  if ( json.containsKey("discardOutliers") )    _discardOutliers    = json["discardOutliers"];
  if ( json.containsKey("setOutliersTo") )      _setOutliersTo      = json["setOutliersTo"];

  if ( json.containsKey("outputRange") ) {
    JsonArray outputRange = json["outputRange"];
    if ( outputRange[MAX] < outputRange[MIN] ) {
      setError("Output range must be positive");
      return false;
    }
    _outputRange[MIN] = outputRange[MIN];
    _outputRange[MAX] = outputRange[MAX];
  }

  if ( json.containsKey("flipRange") )   _flipRange   = json["flipRange"];
  if ( json.containsKey("mirrorRange") ) _mirrorRange = json["mirrorRange"];

  if ( json.containsKey("bandPass") ) {
    JsonArray bandPass = json["bandPass"];
    if ( bandPass[MAX] < bandPass[MIN] ) {
      setError("Band Pass must be positive");
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
      setError("Band Cut must be positive");
      return false;
    }
    _bandCut[MIN] = bandCut[MIN];
    _bandCut[MAX] = bandCut[MAX];
  }

  return true;
}

String ValueTransform::getError() {
  return _error;
}

void ValueTransform::setError(String error) {
  _error = "Bad ValueTransform: " + error;
}

//JsonObject ValueTransform::toJson() {
//  _jsonRoot.clear();
//  JsonObject json = _jsonRoot.to<JsonObject>();
//
//  JsonArray inputRange = json.createNestedArray("inputRange");
//  inputRange.add(_inputRange[MIN]);
//  inputRange.add(_inputRange[MAX]);
//
//  json["discardAllOutliers"] = _discardAllOutliers;
//  json["discardOutliers"]    = _discardOutliers;
//  json["setOutliersTo"]      = _setOutliersTo;
//
//  JsonArray outputRange = json.createNestedArray("outputRange");
//  outputRange.add(_outputRange[MIN]);
//  outputRange.add(_outputRange[MAX]);
//
//  json["flipRange"] = _flipRange;
//  json["mirrorRange"] = _mirrorRange;
//
//  JsonArray bandPass = json.createNestedArray("bandPass");
//  bandPass.add(_bandPass[MIN]);
//  bandPass.add(_bandPass[MAX]);
//
//  JsonArray bandCut = json.createNestedArray("bandCut");
//  bandCut.add(_bandCut[MIN]);
//  bandCut.add(_bandCut[MAX]);
//
//  return json;
//}
