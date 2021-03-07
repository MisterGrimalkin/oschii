#include "Envelope.h"

void Envelope::start(int value) {
  _value = value;
  if ( !isRunning() ) {
    _startedAt = millis();
  }
}

void Envelope::stop() {
  _startedAt = -1;
}

int Envelope::get() {
  if ( _startedAt < 0 ) {
    return _value;
  } else {
    int totalTime = getTotalTime();
    int elapsed = millis() - _startedAt;
    if ( !_loop && elapsed > totalTime ) {
      stop();
      return _value;
    } else {
      elapsed %= totalTime;
      int counter = 0;
      for ( int i=0; i<_nodeIndex; i++ ) {
        EnvelopeNode * node = _nodes[i];
        counter += node->getTime();
        if ( counter > elapsed ) {
          return node->getAmount();
        }
      }
    }
  }
  return _value;
}

bool Envelope::isRunning() {
  return _startedAt >= 0;
}
int Envelope::getTotalTime() {
  int time = 0;
  for ( int i=0; i<_nodeIndex; i++ ) {
    EnvelopeNode * node = _nodes[i];
    time += node->getTime();
  }
  return time;
}

bool Envelope::build(JsonArray array) {
  _nodeIndex = 0;
  _loop = false;

  for ( int i=0; i<array.size(); i++ ) {
    JsonObject nodeJson = array[i];
    if ( nodeJson.containsKey("loop") ) {
      _loop = nodeJson["loop"];
    } else {
      EnvelopeNode * node = new EnvelopeNode();
      if ( !node->build(nodeJson) ) {
        return false;
      }
      _nodes[_nodeIndex++] = node;
    }
  }

  return true;
}

JsonArray Envelope::toJson() {
  _jsonRoot.clear();
  JsonArray array = _jsonRoot.createNestedArray("envelope");
  for ( int i=0; i<_nodeIndex; i++ ) {
    EnvelopeNode * node = _nodes[i];
    array.add(node->toJson());
  }
  JsonObject config = _jsonRoot2.to<JsonObject>();
  config["loop"] = _loop;
  array.add(config);

  return array;
}
