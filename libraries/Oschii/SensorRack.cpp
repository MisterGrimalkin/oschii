#include "SensorRack.h"

const bool ECHO_SENSORS = true;

String SensorRack::buildSensors(JsonArray array) {
  Serial.println("Building Sensors:");

  String errorBuffer = "";
  for ( int i = 0; i < array.size(); i++ ) {
    JsonObject json = array[i];
    errorBuffer += buildSensor(json);
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
    return "RuleTwoError: Don't know type '" + type + "'\n";
  }

  if ( sensor->build(json) ) {
    _sensors[_sensorIndex++] = sensor;

    Serial.println(" " + sensor->toString());
    return "";
  } else {
    return sensor->getError() + "\n";
  }
}

void SensorRack::readSensors() {
  for ( int i=0; i<_sensorIndex; i++ ) {
    Sensor * sensor = _sensors[i];
    sensor->readSensor();
    if ( sensor->hasChanged() ) {
      if ( ECHO_SENSORS ) {
        Serial.print(sensor->getName());
        Serial.print(" --> ");
        Serial.println(sensor->getValue());
      }
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

StaticJsonDocument<1024> doc2;

JsonArray SensorRack::toJson() {

  JsonArray array = doc2.createNestedArray("sensors");
  for ( int i=0; i<_sensorIndex; i++ ) {
    Sensor * sensor = _sensors[i];
    array.add(sensor->toJson());
  }
  return array;
}

String SensorRack::toPrettyJson() {
  String outputStr = "";
  serializeJsonPretty(toJson(), outputStr);
  return outputStr;
}
