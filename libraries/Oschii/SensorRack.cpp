#include "SensorRack.h"

SensorRack::SensorRack(I2CRack * i2cRack) {
  _i2cRack = i2cRack;
  _sensorIndex = 0;
}

String SensorRack::buildSensors(JsonArray array) {
  _sensorIndex = 0;

  Serial.println("== SENSORS ==");

  String errorBuffer = "";
  for ( int i = 0; i < array.size(); i++ ) {
    JsonObject json = array[i];
    errorBuffer += buildSensor(json);
  }
  Serial.print("== Found: ");
  Serial.print(_sensorIndex);
  Serial.println(" ==\n");

  return errorBuffer;
}

String SensorRack::buildSensor(JsonObject json) {
  Sensor * sensor;

  String type = json["type"];

  if ( type == "gpio" ) {
    sensor = new GpioSensor(_i2cRack);

  } else if ( type == "touch" ) {
    sensor = new TouchSensor(_i2cRack);

  } else if ( type == "analog" ) {
    sensor = new AnalogSensor(_i2cRack);

  } else if ( type == "hc-sr04" ) {
    sensor = new HCSRSensor(_i2cRack);

  } else {
    return "RuleTwoError: No type of sensor called '" + type + "'\n";
  }

  if ( sensor->build(json) ) {
    _sensors[_sensorIndex++] = sensor;

    Serial.println(" - " + sensor->toString());
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

Sensor * SensorRack::getSensor(String name) {
  for ( int i=0; i<_sensorIndex; i++ ) {
    Sensor * sensor = _sensors[i];
    if ( sensor->getName() == name ) {
      return sensor;
    }
  }
  return NULL;
}

//JsonArray SensorRack::toJson() {
//  _jsonRoot.clear();
//  JsonArray array = _jsonRoot.createNestedArray("sensors");
//  for ( int i=0; i<_sensorIndex; i++ ) {
//    Sensor * sensor = _sensors[i];
//    array.add(sensor->toJson());
//  }
//  return array;
//}
//
//String SensorRack::toPrettyJson() {
//  String outputStr = "";
//  serializeJsonPretty(toJson(), outputStr);
//  return outputStr;
//}
