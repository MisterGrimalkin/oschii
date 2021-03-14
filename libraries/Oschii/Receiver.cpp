#include "Receiver.h"

Receiver::Receiver(NetworkService * network) {
  _network = network;
}

bool Receiver::build(JsonObject json) {
  _oscPort = 3333;
  _httpPort = 80;

  if ( json.containsKey("name") ) {
    _name = json["name"].as<String>();
  } else {
    _error = "RuleTwoError: Receiver needs a name";
    return false;
  }

  if ( json.containsKey("ip") ) {
    _ip = json["ip"].as<String>();
  } else {
    setError("Must specify 'ip'");
    return false;
  }

  if ( json.containsKey("oscPort") )  _oscPort  = json["oscPort"];
  if ( json.containsKey("httpPort") ) _httpPort = json["httpPort"];

  return true;
}

String Receiver::getName() {
  return _name;
}

String Receiver::toString() {
  return "|" + _name + "| " + _ip + " osc:" + String(_oscPort) + " http:" + String(_httpPort);
}

void Receiver::sendOsc(String address, int value) {
  _network->sendOsc(_ip, _oscPort, address, value);
}

void Receiver::setError(String error) {
  _error = "Receiver '" + _name + "': " + error;
}

String Receiver::getError() {
  return _error;
}

