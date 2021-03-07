#ifndef Envelope_h
#define Envelope_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "EnvelopeNode.h"

#define MAX_ENVELOPE_NODES 256

class Envelope {
  public:
    Envelope() {};

    bool build(JsonArray array);
    JsonArray toJson();

    int get();

    void start(int value);
    void stop();

    bool isRunning();

    int getTotalTime();

  private:
    EnvelopeNode * _nodes[MAX_ENVELOPE_NODES];
    int _nodeIndex, _startedAt;
    bool _loop;
    int _value;
    StaticJsonDocument<64> _jsonRoot;
    StaticJsonDocument<64> _jsonRoot2;

};

#endif
