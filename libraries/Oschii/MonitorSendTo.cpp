#include "MonitorSendTo.h"

MonitorSendTo::MonitorSendTo(RemoteRack * remoteRack) {
  _remoteRack = remoteRack;
}

bool MonitorSendTo::build(JsonObject json) {
  _address = "";

  if ( json.containsKey("~") ) {
    _address = json["~"].as<String>();
    _remote = _remoteRack->getRemote(_address);
  }
}

String MonitorSendTo::getAddress() {
  return _address;
}

void MonitorSendTo::send(int value) {
  if ( _remote != NULL ) {
    _remote->receive(value);
  }
}

JsonObject MonitorSendTo::toJson() {
}