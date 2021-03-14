#include "ReceiverRack.h"

ReceiverRack::ReceiverRack(NetworkService * network) {
  _network = network;
}

ReceiverRack::~ReceiverRack() {
  for ( int i=0; i<_receiverIndex; i++ ) {
    Receiver * receiver = _receivers[i];
    delete receiver;
  }
}

bool ReceiverRack::buildReceivers(JsonArray array) {
  _receiverIndex = 0;

  Serial.println("== RECEIVERS ==");

  String errorBuffer = "";
  for ( int i=0; i<array.size(); i++ ) {
    JsonObject json = array[i];
    errorBuffer += buildReceiver(json);
  }

  Serial.print("== Found: ");
  Serial.print(_receiverIndex);
  Serial.println(" ==\n");

  if ( errorBuffer != "" ) {
    Serial.println("Errors:");
    Serial.println(errorBuffer);
    return false;
  }

  return true;
}

String ReceiverRack::buildReceiver(JsonObject json) {
  Receiver * receiver = new Receiver(_network);
  if ( receiver->build(json) ) {
    _receivers[_receiverIndex++] = receiver;
    Serial.println(" - " + receiver->toString());
    return "";
  } else {
    return receiver->getError() + "\n";
  }
}

Receiver * ReceiverRack::getReceiver(String name) {
  for ( int i=0; i<_receiverIndex; i++ ) {
    Receiver * receiver = _receivers[i];
    if ( receiver->getName() == name ) {
      return receiver;
    }
  }
  return NULL;
}

