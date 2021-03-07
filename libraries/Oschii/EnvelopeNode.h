#ifndef EnvelopeNode_h
#define EnvelopeNode_h

#include <Arduino.h>
#include <ArduinoJson.h>

class EnvelopeNode {
  public:
    EnvelopeNode() {};

    bool build(JsonObject json);

    int getAmount();
    int getTime();

    JsonObject toJson();

  private:
    int _amount, _time;
    StaticJsonDocument<64> _jsonRoot;
};

#endif
