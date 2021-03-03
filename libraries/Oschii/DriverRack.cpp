#include "DriverRack.h"

String DriverRack::buildDrivers(JsonArray array) {
  return "";
}

String DriverRack::buildDriver(JsonObject json) {
  return "";
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
