#define VERSION "0.0.2"
#define BUILD_DATETIME __DATE__ " " __TIME__

#include <Arduino.h>
#include <ArduinoJson.h>

#include <Oschii.h>

SensorRack sensorRack;

void setup() {
  Serial.begin(115200);
  Serial.println("\n - OSCHII -\n");

  StaticJsonDocument<2096> root;
  JsonArray sensorArray = root.createNestedArray("sensors");

  {
    JsonObject json = sensorArray.createNestedObject();
    json["name"] = "Knob";
    json["type"] = "analog";
    json["pin"] = 36;
//    json["samples"] = 1;
//    json["interleave"] = false;
//    JsonArray readingRange = json.createNestedArray("readingRange");
//    readingRange.add(0);
//    readingRange.add(3300);
//    JsonArray valueRange = json.createNestedArray("valueRange");
//    valueRange.add(0);
//    valueRange.add(100);
//    json["flipRange"] = false;
//    JsonArray bandPass = json.createNestedArray("bandPass");
//    bandPass.add(0);
//    bandPass.add(100);
//    JsonArray bandCut = json.createNestedArray("bandCut");
//    bandCut.add(-1);
//    bandCut.add(-1);
  }

  {
    JsonObject json = sensorArray.createNestedObject();
    json["name"] = "Ultrasonic";
    json["type"] = "hc-sr04";
    json["trigPin"] = 13;
    json["echoPin"] = 12;
//    json["discardOutliers"] = false;
//    json["samples"] = 5;
//    json["interleave"] = false;
//    JsonArray readingRange = json.createNestedArray("readingRange");
//    readingRange.add(400);
//    readingRange.add(3000);
//    JsonArray valueRange = json.createNestedArray("valueRange");
//    valueRange.add(0);
//    valueRange.add(10);
//    json["flipRange"] = false;
//    JsonArray bandPass = json.createNestedArray("bandPass");
//    bandPass.add(0);
//    bandPass.add(100);
//    JsonArray bandCut = json.createNestedArray("bandCut");
//    bandCut.add(-1);
//    bandCut.add(-1);
  }

  {
    JsonObject json = sensorArray.createNestedObject();
    json["name"] = "Button 1";
    json["type"] = "gpio";
    json["resistor"] = "down";
    json["pin"] = 32;
    json["onValue"] = 101;
    json["offValue"] = 1;
    json["bounceFilter"] = 100;
//    json["holdOnFilter"] = 1000;
//    json["invert"] = false;
  }

  {
    JsonObject json = sensorArray.createNestedObject();
    json["name"] = "Button 2";
    json["type"] = "gpio";
    json["resistor"] = "up";
    json["pin"] = 33;
    json["onValue"] = 102;
    json["offValue"] = 2;
    json["bounceFilter"] = 100;
//    json["invert"] = true;
  }

  {
    JsonObject json = sensorArray.createNestedObject();
    json["name"] = "Button 3";
    json["type"] = "gpio";
    json["resistor"] = "down";
    json["pin"] = 25;
    json["onValue"] = 103;
    json["offValue"] = 3;
    json["bounceFilter"] = 100;
//    json["invert"] = true;
  }

  {
    JsonObject json = sensorArray.createNestedObject();
    json["name"] = "Button 4";
    json["type"] = "gpio";
    json["resistor"] = "up";
    json["pin"] = 26;
    json["onValue"] = 104;
    json["offValue"] = 4;
    json["bounceFilter"] = 100;
//    json["invert"] = true;
  }

  {
    JsonObject json = sensorArray.createNestedObject();
    json["name"] = "Touch 1";
    json["type"] = "touch";
    json["pin"] = 4;
    json["onValue"] = 501;
    json["offValue"] = 5;
    json["bounceFilter"] = 100;
    json["holdOnFilter"] = 150;
  }

  {
    JsonObject json = sensorArray.createNestedObject();
    json["name"] = "Touch 2";
    json["type"] = "touch";
    json["pin"] = 2;
    json["onValue"] = 601;
    json["offValue"] = 6;
    json["bounceFilter"] = 100;
    json["holdOnFilter"] = 150;
  }

  String error = sensorRack.buildSensors(sensorArray);

  if ( error != "" ) Serial.println(error);

  Serial.println(sensorRack.toPrettyJson());
}

void loop() {
  sensorRack.readSensors();
  delay(1);
}
