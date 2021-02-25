#ifndef SensorRack_h
#define SensorRack_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "Sensor.h"
#include "AnalogSensor.h"
#include "HCSRSensor.h"

class SensorRack {
  public:
    SensorRack() {};
    bool buildSensor(JsonObject json);
    void readAllSensors();

  private:
    Sensor * _sensors[100];
    int _sensorIndex;
};

#endif