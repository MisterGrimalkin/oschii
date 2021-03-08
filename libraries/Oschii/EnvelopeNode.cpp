#include "EnvelopeNode.h"

bool EnvelopeNode::build(JsonObject json) {
  _amount = 100;
  _time = 1000;

  if ( json.containsKey("amount") ) _amount = json["amount"];
  if ( json.containsKey("time") )   _time   = json["time"];

  _fadeToAmount = _amount;
  _interpolation = "cosine";

  if ( json.containsKey("fadeToAmount") ) _fadeToAmount = json["fadeToAmount"];
  if ( json.containsKey("interpolation") ) _interpolation = json["interpolation"].as<String>();

  _amountFrom = -1;
  _fadeToAmountFrom = -1;
  _timeFrom = -1;

  if ( json.containsKey("getAmountFrom") )       _amountFrom       = json["getAmountFrom"];
  if ( json.containsKey("getFadeToAmountFrom") ) _fadeToAmountFrom = json["getFadeToAmountFrom"];
  if ( json.containsKey("getTimeFrom") )         _timeFrom         = json["getTimeFrom"];

  return true;
}

JsonObject EnvelopeNode::toJson() {
  _jsonRoot.clear();
  JsonObject json = _jsonRoot.to<JsonObject>();

  json["amount"] = _amount;
  json["time"] = _time;

  return json;
}

int EnvelopeNode::getAmount(int value) {
  if ( _amountFrom == 0 ) {
    return value;
  } else {
    return _amount;
  }
}


int EnvelopeNode::getFadeToAmount(int value) {
  if ( _fadeToAmountFrom == 0 ) {
    return value;
  } else {
    return _fadeToAmount;
  }
}

int EnvelopeNode::getTime(int value) {
  if ( _timeFrom == 0 ) {
    return value;
  } else {
    return _time;
  }
}

String EnvelopeNode::getInterpolation() {
  return _interpolation;
}
