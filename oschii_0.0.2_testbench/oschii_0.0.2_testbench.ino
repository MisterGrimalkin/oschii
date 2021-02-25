#define VERSION "0.0.2"
#define BUILD_DATETIME __DATE__ " " __TIME__

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Oschii.h>

SensorRack sensorRack;

void setup() {
  Serial.begin(115200);
  Serial.println("\n - OSCHII -\n");

  StaticJsonDocument<512> root;
  JsonArray sensorArray = root.createNestedArray("sensors");

  JsonObject analogJson = sensorArray.createNestedObject();
  analogJson["name"] = "Knob";
  analogJson["type"] = "analog";
  analogJson["pin"] = 36;

  JsonObject hcsrJson = sensorArray.createNestedObject();
  hcsrJson["name"] = "Ultrasonic";
  hcsrJson["type"] = "hc-sr04";
  hcsrJson["samples"] = 5;
  hcsrJson["interleave"] = true;
  hcsrJson["flipRange"] = true;
  hcsrJson["trigPin"] = 13;
  hcsrJson["echoPin"] = 12;

  String error = sensorRack.buildSensors(sensorArray);
  if ( error != "" ) {
    Serial.println(error);
  }

}

void loop() {
  sensorRack.readSensors();
  sensorRack.printSensorValues();
  delay(50);
}
