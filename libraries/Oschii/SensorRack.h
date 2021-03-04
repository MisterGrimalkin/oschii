#ifndef SensorRack_h
#define SensorRack_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "Sensor.h"
#include "GpioSensor.h"
#include "TouchSensor.h"
#include "AnalogSensor.h"
#include "HCSRSensor.h"

#define MAX_SENSORS 512

#define ECHO_SENSORS true

class SensorRack {
  public:
    SensorRack();
    String buildSensors(JsonArray array);
    String buildSensor(JsonObject json);
    void readSensors();
    void printSensorValues();
    JsonArray toJson();
    String toPrettyJson();

    Sensor * getSensor(String name);

  private:
    Sensor * _sensors[MAX_SENSORS];
    int _sensorIndex;
};

#endif
