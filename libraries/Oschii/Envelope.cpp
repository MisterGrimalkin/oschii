#include "Envelope.h"

bool Envelope::start(int value) {
  if ( value != _triggerValue ) {
    _value = value;
  }
  if ( ( _triggerValue==-1 || value==_triggerValue)
      && !isRunning() ) {
    _startedAt = millis();
    return _triggerValue > 0;
  }
  return false;
}

void Envelope::stop() {
  _startedAt = -1;
}

double linearInterpolate(double v1, double v2, double amount) {
  return v1 + (amount * (v2 - v1));
}

double cosineInterpolate(double v1, double v2, double amount) {
  double mu = (1-cos(amount*PI))/2;
  return(v1*(1-mu)+v2*mu);
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
        int amount = node->getAmount(_value);
        int fadeToAmount = node->getFadeToAmount(_value);
        int time = node->getTime(_value);

        if ( counter + time > elapsed ) {
          if ( fadeToAmount < 0 ) {
            return amount;
          } else {
            String interpolation = node->getInterpolation();

            double progress = (double)(elapsed - counter) / time;

            if ( interpolation == "cosine" ) {
              return cosineInterpolate(
                        amount, fadeToAmount, progress
                     );
            } else {
              return linearInterpolate(
                        amount, fadeToAmount, progress
                     );
            }
          }
        }
        counter += time;
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
    time += node->getTime(_value);
  }
  return time;
}

bool Envelope::build(JsonArray array) {
  _startedAt = -1;
  _nodeIndex = 0;
  _triggerValue = -1;
  _loop = false;

  for ( int i=0; i<array.size(); i++ ) {
    JsonObject nodeJson = array[i];
    bool isConfigNode = false;

    if ( nodeJson.containsKey("loop") ) {
      _loop = nodeJson["loop"];
      isConfigNode = true;
    }

    if ( nodeJson.containsKey("triggerValue") ) {
      _triggerValue = nodeJson["triggerValue"];
      isConfigNode = true;
    }

    if ( !isConfigNode ) {
      EnvelopeNode * node = new EnvelopeNode();
      if ( !node->build(nodeJson) ) {
        setError(node->getError());
        return false;
      }
      _nodes[_nodeIndex++] = node;
    }
  }

  return true;
}

void Envelope::setError(String error) {
  _error = "Bad Envelope: " + error;
}

String Envelope::getError() {
  return _error;
}

//JsonArray Envelope::toJson() {
//  _jsonRoot.clear();
//  JsonArray array = _jsonRoot.createNestedArray("envelope");
//  for ( int i=0; i<_nodeIndex; i++ ) {
//    EnvelopeNode * node = _nodes[i];
//    array.add(node->toJson());
//  }
//  JsonObject config = _jsonRoot2.to<JsonObject>();
//  config["loop"] = _loop;
//  array.add(config);
//
//  return array;
//}
