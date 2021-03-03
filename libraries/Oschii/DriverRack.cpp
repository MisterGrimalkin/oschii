#include "DriverRack.h"

String DriverRack::buildDrivers(JsonArray array) {
  Serial.println("Building Drivers:");

  String errorBuffer = "";
  for ( int i = 0; i < array.size(); i++ ) {
    JsonObject json = array[i];
    errorBuffer += buildDriver(json);
  }

  Serial.print("Found: ");
  Serial.print(_driverIndex);
  Serial.println("\n");

  return errorBuffer;}

String DriverRack::buildDriver(JsonObject json) {
  Driver * driver;

  String type = json["type"];

  if ( false ) {

  } else {
    return "RuleTwoError: No type of driver called '" + type + "'\n";
  }

  if ( driver->build(json) ) {
    _drivers[_driverIndex++] = driver;

    Serial.println(" " + driver->toString());
    return "";
  } else {
    return driver->getError() + "\n";
  }
}

StaticJsonDocument<4096> doc3;

JsonArray DriverRack::toJson() {
  JsonArray array = doc3.createNestedArray("drivers");
  for ( int i=0; i<_driverIndex; i++ ) {
    Driver * driver = _drivers[i];
    array.add(driver->toJson());
  }
  return array;
}

String DriverRack::toPrettyJson() {
  String outputStr = "";
  serializeJsonPretty(toJson(), outputStr);
  return outputStr;
}
