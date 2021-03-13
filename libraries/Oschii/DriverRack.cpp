#include "DriverRack.h"

DriverRack::DriverRack(I2CRack * i2cRack) {
  _i2cRack = i2cRack;
  _driverIndex = 0;
}

String DriverRack::buildDrivers(JsonArray array) {
  _driverIndex = 0;

  Serial.println("== DRIVERS ==");

  String errorBuffer = "";
  for ( int i = 0; i < array.size(); i++ ) {
    JsonObject json = array[i];
    errorBuffer += buildDriver(json);
  }

  Serial.print("== Found: ");
  Serial.print(_driverIndex);
  Serial.println(" ==\n");

  if ( errorBuffer != "" ) {
    Serial.println("Errors:");
    Serial.println(errorBuffer);
  }

  return errorBuffer;
}

String DriverRack::buildDriver(JsonObject json) {
  Driver * driver;

  String type = json["type"];

  if ( type == "gpio" ) {
    driver = new GpioDriver(_i2cRack);

  } else if ( type == "pwm" ) {
    driver = new PwmDriver(_i2cRack);

  } else {
    return "RuleTwoError: No type of driver called '" + type + "'\n";
  }

  if ( driver->build(json) ) {
    _drivers[_driverIndex++] = driver;

    Serial.println(" - " + driver->toString());
    return "";
  } else {
    return driver->getError() + "\n";
  }
}

void DriverRack::fireAll(int value) {
  for ( int i=0; i<_driverIndex; i++ ) {
    Driver * driver = _drivers[i];
    driver->fire(value);
  }
}

Driver * DriverRack::getDriver(String name) {
  for ( int i=0; i<_driverIndex; i++ ) {
    Driver * driver = _drivers[i];
    if ( driver->getName() == name ) {
      return driver;
    }
  }
  return NULL;
}

//JsonArray DriverRack::toJson() {
//  _jsonRoot.clear();
//  JsonArray array = _jsonRoot.createNestedArray("drivers");
//  for ( int i=0; i<_driverIndex; i++ ) {
//    Driver * driver = _drivers[i];
//    array.add(driver->toJson());
//  }
//  return array;
//}
//
//String DriverRack::toPrettyJson() {
//  String outputStr = "";
//  serializeJsonPretty(toJson(), outputStr);
//  return outputStr;
//}
