#include "SensorRack.h"

bool SensorRack::buildSensor(JsonObject json) {
  Sensor * sensor;

  static AnalogSensor analogSensor;
  static HCSRSensor hcsrSensor;

  if ( json["type"] == "analog" ) {
    analogSensor = AnalogSensor(_sensorIndex);
    sensor = &analogSensor;

  } else if ( json["type"] == "hc-sr04" ) {
    hcsrSensor = HCSRSensor(_sensorIndex);
    sensor = &hcsrSensor;
  }

  if ( sensor->build(json) ) {
    Serial.println(sensor->toString());
    _sensors[_sensorIndex++] = sensor;
    return true;
  } else {
    Serial.println(sensor->getError());
    return false;
  }
}

void SensorRack::readAllSensors() {
  for ( int i=0; i<_sensorIndex; i++ ) {
    Sensor * sensor = _sensors[i];
    sensor->readSensor();
    if ( sensor->hasChanged() ) {
      Serial.print(sensor->getName());
      Serial.print(" : ");
      Serial.println(sensor->getValue());
    }
  }
}