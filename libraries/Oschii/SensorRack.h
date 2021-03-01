#ifndef SensorRack_h
#define SensorRack_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "Sensor.h"
#include "GpioSensor.h"
#include "TouchSensor.h"
#include "AnalogSensor.h"
#include "HCSRSensor.h"

#define MAX_SENSORS 20

class SensorRack {
  public:
    SensorRack() {};
    String buildSensors(JsonArray array);
    String buildSensor(JsonObject json);
    void readSensors();
    void printSensorValues();
    JsonArray toJson();
    String toPrettyJson();

  private:
    Sensor * _sensors[MAX_SENSORS];
    GpioSensor _gpioSensors[MAX_SENSORS];
    TouchSensor _touchSensors[MAX_SENSORS];
    AnalogSensor _analogSensors[MAX_SENSORS];
    HCSRSensor _hcsrSensors[MAX_SENSORS];
    int _sensorIndex;
};

#endif