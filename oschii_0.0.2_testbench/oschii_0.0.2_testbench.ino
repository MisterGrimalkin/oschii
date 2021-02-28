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
  analogJson["samples"] = 1;
  analogJson["interleave"] = false;
  analogJson["pin"] = 36;
  JsonArray readingRange = analogJson.createNestedArray("readingRange");
  readingRange.add(0);
  readingRange.add(3300);
  JsonArray valueRange = analogJson.createNestedArray("valueRange");
  valueRange.add(0);
  valueRange.add(100);
  analogJson["flipRange"] = false;
  JsonArray bandPass = analogJson.createNestedArray("bandPass");
  bandPass.add(0);
  bandPass.add(100);
  JsonArray bandCut = analogJson.createNestedArray("bandCut");
  bandCut.add(0);
  bandCut.add(0);

//  JsonObject hcsrJson = sensorArray.createNestedObject();
//  hcsrJson["name"] = "Ultrasonic";
//  hcsrJson["type"] = "hc-sr04";
//  hcsrJson["samples"] = 5;
//  hcsrJson["interleave"] = true;
//  hcsrJson["flipRange"] = true;
//  hcsrJson["trigPin"] = 13;
//  hcsrJson["echoPin"] = 12;

  String error = sensorRack.buildSensors(sensorArray);

  if ( error != "" ) {
    Serial.println(error);
  }

  Serial.println(sensorRack.toPrettyJson());
}

void loop() {
  sensorRack.readSensors();
//  sensorRack.printSensorValues();
//  delay(50);
}
