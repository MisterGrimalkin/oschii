#include "EnvelopeNode.h"

bool EnvelopeNode::build(JsonObject json) {
  _amount = 100;
  _time = 1000;

  if ( json.containsKey("amount") ) _amount = json["amount"];
  if ( json.containsKey("time") )   _time   = json["time"];

  return true;
}

JsonObject EnvelopeNode::toJson() {
  _jsonRoot.clear();
  JsonObject json = _jsonRoot.to<JsonObject>();

  json["amount"] = _amount;
  json["time"] = _time;

  return json;
}

int EnvelopeNode::getAmount() {
  return _amount;
}

int EnvelopeNode::getTime() {
  return _time;
}
