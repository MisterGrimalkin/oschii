#ifndef Envelope_h
#define Envelope_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "EnvelopeNode.h"

#define MAX_ENVELOPE_NODES 256

class Envelope {
  public:
    Envelope() {};
    ~Envelope();

    bool build(JsonArray array);

    int get();

    bool start(int value);
    void stop();
    bool isRunning();

    int getTotalTime();

    void setError(String error);
    String getError();

//    JsonArray toJson();

  private:
    EnvelopeNode * _nodes[MAX_ENVELOPE_NODES];
    int _nodeIndex, _triggerValue, _startedAt;
    bool _loop;
    int _value;
    String _error;

//    StaticJsonDocument<64> _jsonRoot;
//    StaticJsonDocument<64> _jsonRoot2;
};

#endif
