#ifndef RemoteWriteTo_h
#define RemoteWriteTo_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "DriverRack.h"
#include "ValueTransform.h"
#include "Envelope.h"

class RemoteWriteTo {
  public:
    RemoteWriteTo(DriverRack * driverRack);

    bool build(JsonObject json);
    void write(int value);
    void write(int value, bool fromEnvelope);

    void update();

    String toString();

    String getDriverName();

    String getError();
    void setError(String error);

//    JsonObject toJson();

  private:
    DriverRack * _driverRack;
    Driver * _driver;
    Envelope * _envelope;
    ValueTransform * _transform;
    String _error;
//    StaticJsonDocument<128> _jsonRoot;
};

#endif
