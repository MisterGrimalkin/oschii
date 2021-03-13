#include "MonitorSendTo.h"

MonitorSendTo::MonitorSendTo(RemoteRack * remoteRack) {
  _remoteRack = remoteRack;
  _remote = NULL;
  _transform = NULL;
}

void MonitorSendTo::send(int value) {
  int sendValue = value;
  if ( _transform != NULL ) {
    sendValue = _transform->apply(value);
  }
  if ( _remote != NULL ) {
    _remote->receive(sendValue);
  }
}

bool MonitorSendTo::build(JsonObject json) {
  _address = "";

  if ( json.containsKey("~") ) {
    _address = json["~"].as<String>();
    _remote = _remoteRack->getRemote(_address);
  }

  if ( json.containsKey("valueTransform") ) {
    JsonObject transformJson = json["valueTransform"];
    _transform = new ValueTransform();
    _transform->build(transformJson);
  }
}

String MonitorSendTo::getAddress() {
  return _address;
}

//JsonObject MonitorSendTo::toJson() {
//  _jsonRoot.clear();
//  JsonObject json = _jsonRoot.to<JsonObject>();
//
//  if ( _remote != NULL ) {
//    json["~"] = _address;
//  }
//
//  if ( _transform != NULL ) {
//    json["valueTransform"] = _transform->toJson();
//  }
//
//  return json;
//}
