#ifndef SensorRack_h
#define SensorRack_h

#include <Arduino.h>
#include <ArduinoJson.h>

#include "Sensor.h"
#include "GpioSensor.h"
#include "TouchSensor.h"
#include "AnalogSensor.h"
#include "HCSRSensor.h"
#include "I2CRack.h"

#define MAX_SENSORS 512

#define ECHO_SENSORS true

class SensorRack {
  public:
    SensorRack(I2CRack * i2cRack);
    String buildSensors(JsonArray array);
    String buildSensor(JsonObject json);
    void readSensors();
    void printSensorValues();
    JsonArray toJson();
    String toPrettyJson();

    Sensor * getSensor(String name);

  private:
    StaticJsonDocument<128> _jsonRoot;
    Sensor * _sensors[MAX_SENSORS];
    I2CRack * _i2cRack;
    int _sensorIndex;
};

#endif
