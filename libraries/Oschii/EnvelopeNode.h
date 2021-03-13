#ifndef EnvelopeNode_h
#define EnvelopeNode_h

#include <Arduino.h>
#include <ArduinoJson.h>

class EnvelopeNode {
  public:
    EnvelopeNode() {};

    bool build(JsonObject json);

    int getAmount(int value);
    int getFadeToAmount(int value);
    int getTime(int value);
    String getInterpolation();

//    JsonObject toJson();

  private:
    int _amount, _fadeToAmount, _time;
    int _amountFrom, _fadeToAmountFrom, _timeFrom;
    String _interpolation;

//    StaticJsonDocument<64> _jsonRoot;
};

#endif
