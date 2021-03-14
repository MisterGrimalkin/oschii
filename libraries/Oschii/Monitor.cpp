#include "Monitor.h"

Monitor::Monitor(SensorRack * sensorRack, RemoteRack * remoteRack) {
  _sensorRack = sensorRack;
  _remoteRack = remoteRack;
  _transform = NULL;
  _sendToIndex = 0;
}

Monitor::~Monitor() {
  if ( _transform != NULL ) delete _transform;
  for ( int i=0; i<_sendToIndex; i++ ) {
    MonitorSendTo * sendTo = _sendTos[i];
    delete sendTo;
  }
}

void Monitor::update() {
  _sensor->readSensor();
  if ( _onChange && _sensor->hasChanged() ) {
    for ( int i=0; i<_sendToIndex; i++ ) {
      int value = applyTransform(_sensor->getValue());

      MonitorSendTo * sendTo = _sendTos[i];
      if ( ECHO_MONITOR_SENDS ) {
        Serial.print("(");
        Serial.print(_sensor->getName());
        Serial.print(") --> ");
        Serial.println(value);
      }
      sendTo->send(value);
    }
    _lastPolledAt = millis();
  }
}

bool Monitor::build(JsonObject json) {
  _onChange = true;
  _pollInterval = 0;
  _lastPolledAt = millis();

  if ( json.containsKey("sensor") ) {
    String sensorName = json["sensor"];
    _sensor = _sensorRack->getSensor(sensorName);
    if ( _sensor == NULL ) {
      setError("Sensor '" + sensorName + "' not found");
      return false;
    }
  } else {
    setError("Must specify Sensor");
    return false;
  }

  if ( json.containsKey("valueTransform") ) {
    JsonObject transformJson = json["valueTransform"];
    _transform = new ValueTransform();
    if ( !_transform->build(transformJson) ) {
      setError(_transform->getError());
      return false;
    }
  }

  if ( json.containsKey("onChange") )     _onChange     = json["onChange"];
  if ( json.containsKey("pollInterval") ) _pollInterval = json["pollInterval"];

  JsonArray sendToArray = json["sendTo"];

  for ( int i=0; i<sendToArray.size(); i++ ) {
    JsonObject sendToJson = sendToArray[i];
    MonitorSendTo * sendTo = new MonitorSendTo(_remoteRack);
    if (sendTo->build(sendToJson)) {
      _sendTos[_sendToIndex++] = sendTo;
    } else {
      setError(sendTo->getError());
      return false;
    }
  }

  return true;
}

int Monitor::applyTransform(int value) {
  if ( _transform == NULL ) {
    return value;
  } else {
    return _transform->apply(value);
  }
}

String Monitor::toString() {
  String str = "(" + _sensor->getName() + ") ~~>";
  for ( int i=0; i<_sendToIndex; i++ ) {
    MonitorSendTo * sendTo = _sendTos[i];
    str += " <" + sendTo->getAddress() + ">";
  }
  return str;
}

String Monitor::getError() {
  return _error;
}

void Monitor::setError(String error) {
  _error = "Monitor: " + error;
}
//JsonObject Monitor::toJson() {
//  _jsonRoot.clear();
//  JsonObject json = _jsonRoot.to<JsonObject>();
//
//  json["sensor"] = _sensor->getName();
//  json["onChange"] = _onChange;
//  json["pollInterval"] = _pollInterval;
//
//  JsonArray sendToArray = json.createNestedArray("sendTo");
//  for ( int i=0; i<_sendToIndex; i++ ) {
//    MonitorSendTo * sendTo = _sendTos[i];
//    sendToArray.add(sendTo->toJson());
//  }
//
//  return json;
//}
