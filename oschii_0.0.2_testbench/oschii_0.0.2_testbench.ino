#define VERSION "0.0.2"
#define BUILD_DATETIME __DATE__ " " __TIME__

#include <Arduino.h>
#include <ArduinoJson.h>

#include <Oschii.h>

SensorRack sensorRack;
DriverRack driverRack;

Sensor * testSensor1;
Sensor * testSensor2;
Sensor * knob;

Driver * testDriver;

void setup() {
  Serial.begin(115200);
  Serial.println("\n - OSCHII -\n");

  JsonArray sensorArray = testSensors();
  String error = sensorRack.buildSensors(sensorArray);
  if ( error != "" ) Serial.println(error);

  JsonArray driverArray = testDrivers();
  error = driverRack.buildDrivers(driverArray);
  if ( error != "" ) Serial.println(error);

//  Serial.println(sensorRack.toPrettyJson());
//  Serial.println(driverRack.toPrettyJson());

  testSensor1 = sensorRack.getSensor("Button 1");
  testSensor2 = sensorRack.getSensor("Button 2");
  knob = sensorRack.getSensor("Knob");
  testDriver = driverRack.getDriver("Red GPIO");
}

void loop() {
  sensorRack.readSensors();

  if ( knob->hasChanged() ) {
    driverRack.fireAll(knob->getValue());
  }


  if ( testSensor1->hasChanged() ) {
//    testDriver->fire(testSensor->getValue());
    driverRack.fireAll(testSensor1->getValue());
  }
  if ( testSensor2->hasChanged() ) {
//    testDriver->fire(testSensor->getValue());
    driverRack.fireAll(testSensor2->getValue());
  }
  delay(1);
}

StaticJsonDocument<2096> root;

JsonArray testDrivers() {
  JsonArray array = root.createNestedArray("drivers");

  {
    JsonObject json = array.createNestedObject();
    json["name"] = "Red GPIO";
    json["type"] = "gpio",
    json["pin"] = 16;
    json["thresholdValue"] = 50;
    json["initialValue"] = 0;
  }

  {
    JsonObject json = array.createNestedObject();
    json["name"] = "Orange GPIO";
    json["type"] = "gpio",
    json["pin"] = 17;
    json["thresholdValue"] = 50;
    json["initialValue"] = 0;
  }

  {
    JsonObject json = array.createNestedObject();
    json["name"] = "Yellow GPIO";
    json["type"] = "gpio",
    json["pin"] = 18;
    json["thresholdValue"] = 50;
    json["initialValue"] = 0;
  }

  {
    JsonObject json = array.createNestedObject();
    json["name"] = "Green PWM";
    json["type"] = "pwm",
    json["pin"] = 19;
  }

  {
    JsonObject json = array.createNestedObject();
    json["name"] = "Blue PWM";
    json["type"] = "pwm",
    json["pin"] = 21;
  }

  {
    JsonObject json = array.createNestedObject();
    json["name"] = "Indigo PWM";
    json["type"] = "pwm",
    json["pin"] = 22;
  }

  {
    JsonObject json = array.createNestedObject();
    json["name"] = "Violet PWM";
    json["type"] = "pwm",
    json["pin"] = 23;
  }

  return array;
}

JsonArray testSensors() {
  JsonArray array = root.createNestedArray("sensors");

  {
    JsonObject json = array.createNestedObject();
    json["name"] = "Knob";
    json["type"] = "analog";
    json["pin"] = 36;
    JsonArray valueRange = json.createNestedArray("valueRange");
    valueRange.add(0);
    valueRange.add(255);
  }

  {
    JsonObject json = array.createNestedObject();
    json["name"] = "Ultrasonic";
    json["type"] = "hc-sr04";
    json["trigPin"] = 13;
    json["echoPin"] = 12;
  }

  {
    JsonObject json = array.createNestedObject();
    json["name"] = "Button 1";
    json["type"] = "gpio";
    json["resistor"] = "down";
    json["pin"] = 32;
    json["onValue"] = 101;
    json["offValue"] = -1;
    json["bounceFilter"] = 100;
  }

  {
    JsonObject json = array.createNestedObject();
    json["name"] = "Button 2";
    json["type"] = "gpio";
    json["resistor"] = "up";
    json["pin"] = 33;
    json["onValue"] = 102;
    json["offValue"] = 2;
    json["bounceFilter"] = 100;
    json["invert"] = true;
  }

  {
    JsonObject json = array.createNestedObject();
    json["name"] = "Button 3";
    json["type"] = "gpio";
    json["resistor"] = "down";
    json["pin"] = 25;
    json["onValue"] = 103;
    json["offValue"] = 3;
    json["bounceFilter"] = 100;
  }

  {
    JsonObject json = array.createNestedObject();
    json["name"] = "Button 4";
    json["type"] = "gpio";
    json["resistor"] = "up";
    json["pin"] = 26;
    json["onValue"] = 104;
    json["offValue"] = 4;
    json["bounceFilter"] = 100;
    json["invert"] = true;
  }

  {
    JsonObject json = array.createNestedObject();
    json["name"] = "Touch 1";
    json["type"] = "touch";
    json["pin"] = 4;
    json["thresholdValue"] = 30;
    json["onValue"] = 501;
    json["offValue"] = 5;
    json["bounceFilter"] = 100;
    json["holdOnFilter"] = 150;
  }

  {
    JsonObject json = array.createNestedObject();
    json["name"] = "Touch 2";
    json["type"] = "touch";
    json["pin"] = 2;
    json["thresholdValue"] = 30;
    json["onValue"] = 601;
    json["offValue"] = 6;
    json["bounceFilter"] = 100;
    json["holdOnFilter"] = 150;
  }

  return array;
}
