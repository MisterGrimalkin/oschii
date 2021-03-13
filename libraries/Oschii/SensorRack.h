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
    ~SensorRack();

    String buildSensors(JsonArray array);
    String buildSensor(JsonObject json);

    void readSensors();

    Sensor * getSensor(String name);

//    JsonArray toJson();
//    String toPrettyJson();

  private:
    Sensor * _sensors[MAX_SENSORS];
    I2CRack * _i2cRack;
    int _sensorIndex;
//    StaticJsonDocument<128> _jsonRoot;
};

#endif
