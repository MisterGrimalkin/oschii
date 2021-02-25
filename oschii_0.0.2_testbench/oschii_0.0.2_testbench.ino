#define VERSION "0.0.2"
#define BUILD_DATETIME __DATE__ " " __TIME__

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Oschii.h>

SensorRack sensorRack;

void setup() {
  Serial.begin(115200);
  Serial.println("\n - OSCHII -\n");

  StaticJsonDocument<512> analogDoc;
  JsonObject analogJson = analogDoc.to<JsonObject>();
  analogJson["name"] = "Test Analog";
  analogJson["type"] = "analog";
  analogJson["pin"] = 36;
  if ( !sensorRack.buildSensor(analogJson) ) {
    Serial.println("BLAH!");
  }

  StaticJsonDocument<512> hcsrDoc;
  JsonObject hcsrJson = hcsrDoc.to<JsonObject>();
  hcsrJson["name"] = "Test Ultrasonic";
  hcsrJson["type"] = "hc-sr04";
  hcsrJson["samples"] = 9;
  hcsrJson["interleave"] = true;
  hcsrJson["flipRange"] = true;
  hcsrJson["trigPin"] = 13;
  hcsrJson["echoPin"] = 12;
  if ( !sensorRack.buildSensor(hcsrJson) ) {
    Serial.println("BLAH!");
  }

  Serial.println("Go!");
}

void loop() {
  sensorRack.readAllSensors();
  delay(5);
}
