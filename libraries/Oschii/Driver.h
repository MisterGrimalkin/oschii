#ifndef Driver_h
#define Driver_h

#include <Arduino.h>
#include <ArduinoJson.h>

class Driver {
  public:
    Driver();

    virtual void fire(int value) {};
    virtual bool build(JsonObject json);

    String toPrettyJson();
    virtual JsonObject toJson();
    virtual String toString();

    int getValue();
    String getType();
    String getName();
    String getError();

  protected:
    int _value;
    bool _built;
    String _name, _type, _error;
    StaticJsonDocument<512> _jsonRoot;

    void setError(String error);
};

#endif
