#ifndef ValueTransform_h
#define ValueTransform_h

#include <Arduino.h>
#include <ArduinoJson.h>

#define MIN 0
#define MAX 1
#define MID 2

class ValueTransform {
  public:
    ValueTransform() {};

    int apply(int value);

    int capInput(int input);
    int mapToOutput(int input);

    void setInputRange(int min, int max);
    void setInputRange(int min, int max, int discardOutliers);
    void setOutputRange(int min, int max);
    void setOutputRange(int min, int max, bool flipRange);
    void setBandPass(int min, int max);
    void setBandCut(int min, int max);

    bool build(JsonObject json);
    JsonObject toJson();

    String getError();
    void setError(String error);

  private:
    int _lastInput;
    int _inputRange[2];

    bool _discardAllOutliers;
    int _discardOutliers;
    int _outlierCount;
    int _setOutliersTo;

    int _outputRange[3];
    bool _flipRange, _mirrorRange;

    int _bandPass[2];
    int _bandCut[2];

    StaticJsonDocument<256> _jsonRoot;

    String _error;
};

#endif
