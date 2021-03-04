#include "Remote.h"

Remote::Remote(DriverRack * driverRack) {
  _driverRack = driverRack;
  _driverIndex = 0;
}

bool Remote::build(String name, JsonObject json) {
  _name = name;

  JsonArray driverArray = json["writeTo"];
  if ( driverArray.size() > MAX_REMOTE_DRIVERS ) return false;

  for ( int i=0; i<driverArray.size(); i++ ) {
    JsonObject driverJson = driverArray[i];

    Driver * driver = _driverRack->getDriver(driverJson["driver"]);
    int offset = 0;
    double multiplier = 1.0;

    if ( driverJson.containsKey("valueOffset") )     offset     = driverJson["valueOffset"];
    if ( driverJson.containsKey("valueMultiplier") ) multiplier = driverJson["valueMultiplier"];

    _drivers[_driverIndex] = driver;
    _offsets[_driverIndex] = offset;
    _multipliers[_driverIndex] = multiplier;
    _driverIndex++;
  }

  return true;
}


String Remote::getName() {
  return _name;
}

void Remote::receive(int value) {
  for ( int i=0; i<_driverIndex; i++ ) {
    Driver * driver = _drivers[i];
    driver->fire((value * _multipliers[i]) + _offsets[i]);
  }
}

String Remote::toString() {
  String str = "Remote[" + _name + "] drivers("
                + String(_driverIndex) + "):";
  for ( int i=0; i<_driverIndex; i++ ) {
    Driver * driver = _drivers[i];
    str += driver->getName() + " ";
  }
  return str;
}