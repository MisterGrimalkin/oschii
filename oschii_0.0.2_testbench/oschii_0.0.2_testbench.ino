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
//    json["onValue"] = 100;
//    json["offValue"] = 75;
//    json["bounceFilter"] = 1000;
//    json["invert"] = false;
  }

  {
    JsonObject json = sensorArray.createNestedObject();
    json["name"] = "Button 2";
    json["type"] = "gpio";
    json["resistor"] = "up";
    json["pin"] = 33;
//    json["onValue"] = 0;
//    json["offValue"] = 25;
//    json["bounceFilter"] = 100;
//    json["invert"] = true;
  }

  {
//    JsonObject json = sensorArray.createNestedObject();
//    json["name"] = "Touch 1";
//    json["type"] = "touch";
//    json["pin"] = 4;
//    json["onValue"] = 44;
//    json["offValue"] = 25;
//    json["bounceFilter"] = 100;
//    json["invert"] = true;
  }

  {
//    JsonObject json = sensorArray.createNestedObject();
//    json["name"] = "Touch 2";
//    json["type"] = "touch";
//    json["pin"] = 2;
//    json["triggerThreshold"] = 10;
//    json["onValue"] = 88;
//    json["offValue"] = 25;
//    json["bounceFilter"] = 100;
//    json["invert"] = true;
  }

  String error = sensorRack.buildSensors(sensorArray);

  if ( error != "" ) {
    Serial.println(error);
  }

//  Serial.println(sensorRack.toPrettyJson());
}

void loop() {
  sensorRack.readSensors();
//  sensorRack.printSensorValues();
  delay(1);
}
