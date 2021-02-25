#include "SensorRack.h"

String SensorRack::buildSensors(JsonArray array) {
  String errorBuffer = "";
  Serial.println("Building Sensors:");
  for ( int i = 0; i < array.size(); i++ ) {
    JsonObject json = array[_sensorIndex];
    String error = buildSensor(json);
    if ( error != "" ) {
      errorBuffer += error + "\n";
    }
  }
  Serial.print("Found: ");
  Serial.print(_sensorIndex);
  Serial.println("\n");
  return errorBuffer;
}

String SensorRack::buildSensor(JsonObject json) {
  Sensor * sensor;

  static AnalogSensor analogSensor;
  static HCSRSensor hcsrSensor;

  String type = json["type"];
  if ( type == "analog" ) {
    analogSensor = AnalogSensor(_sensorIndex);
    sensor = &analogSensor;
  } else if ( type == "hc-sr04" ) {
    hcsrSensor = HCSRSensor(_sensorIndex);
    sensor = &hcsrSensor;
  } else {
    return "What is this thing called " + type + "?";
  }

  if ( sensor->build(json) ) {
    Serial.println(" " + sensor->toString());
    _sensors[_sensorIndex++] = sensor;
    return "";
  } else {
    return sensor->getError();
  }
}

void SensorRack::readSensors() {
  for ( int i=0; i<_sensorIndex; i++ ) {
    Sensor * sensor = _sensors[i];
    sensor->readSensor();
    if ( sensor->hasChanged() ) {
    }
  }
}

int printCount = 0;

void SensorRack::printSensorValues() {
  String headerRow = " - ";
  String valueRow = "";
  for ( int i=0; i<_sensorIndex; i++ ) {
    Sensor * sensor = _sensors[i];
    if ( printCount == 0 ) {
      headerRow += sensor->getName() + " - ";
    }
    if ( sensor->hasChanged() ) {
      valueRow += " - *";
    } else {
      valueRow += " -  ";
    }
    valueRow += String(sensor->getValue());
  }
  if ( printCount == 0 ) {
    Serial.println();
    Serial.println(headerRow);
    Serial.println();
  }
  Serial.println(valueRow);
  if ( printCount > 30 ) {
    printCount = 0;
  } else {
    printCount++;
  }
}