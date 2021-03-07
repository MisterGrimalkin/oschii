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

    JsonObject toJson();
    String toString();

    String getDriverName();

  private:
    DriverRack * _driverRack;
    Driver * _driver;
    Envelope * _envelope;
    StaticJsonDocument<128> _jsonRoot;
    ValueTransform * _transform;
};

#endif
