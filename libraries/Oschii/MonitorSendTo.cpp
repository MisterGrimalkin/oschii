#include "MonitorSendTo.h"

MonitorSendTo::MonitorSendTo(RemoteRack * remoteRack, ReceiverRack * receiverRack) {
  _remoteRack = remoteRack;
  _receiverRack = receiverRack;
  _remote = NULL;
  _receiver = NULL;
  _sendToAll = false;
  _transform = NULL;
}

MonitorSendTo::~MonitorSendTo() {
  if ( _transform != NULL ) delete _transform;
}

void MonitorSendTo::send(int value) {
  int sendValue = value;
  if ( _transform != NULL ) {
    sendValue = _transform->apply(value);
  }
  if ( _sendToAll ) {
    if ( _type == "osc" ) {
      _receiverRack->sendOscToAll(_address, sendValue);
    } else if ( _type == "http" ) {
      // TODO: send HTTP
    }
  }
  if ( _receiver != NULL ) {
    if ( _type == "osc" ) {
      _receiver->sendOsc(_address, sendValue);
    } else if ( _type == "http" ) {
      // TODO: send HTTP
    }
  }
  if ( _remote != NULL ) {
    _remote->receive(sendValue);
  }
}

bool MonitorSendTo::build(JsonObject json) {
  _address = "";
  _type = "osc";

  if ( json.containsKey("~") ) {
    _address = json["~"].as<String>();
    _remote = _remoteRack->getRemote(_address);
    if ( _remote == NULL ) {
      setError("Remote '" + _address + "' not found");
      return false;
    }

  } else if ( json.containsKey("*") ) {
    _address = json["*"].as<String>();
    _sendToAll = true;

  } else if ( json.containsKey("receiver") ) {
    String receiverName = json["receiver"].as<String>();
    _receiver = _receiverRack->getReceiver(receiverName);
    if ( _receiver == NULL ) {
      setError("Receiver '" + receiverName + "' not found");
      return false;
    }
    if ( json.containsKey("address") ) {
      _address = json["address"].as<String>();
    } else {
      setError("Must specify value for 'address' or '*' or '~'");
      return false;
    }

  } else {
    setError("Nowhere for messages to go!");
    return false;
  }

  if ( json.containsKey("type") ) _type = json["type"].as<String>();

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

String MonitorSendTo::getAddress() {
  String result = _address;
  if ( _sendToAll ) {
    result = "*" + result;
  } else if ( _remote != NULL ) {
    result = "~" + result;
  } else if ( _receiver != NULL) {
    result = _receiver->getName() + result;
  }
  return result;
}

String MonitorSendTo::getError() {
  return _error;
}

void MonitorSendTo::setError(String error) {
  _error = "Monitor Send To: " + error;
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
