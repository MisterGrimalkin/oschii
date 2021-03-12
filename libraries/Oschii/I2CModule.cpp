#include "I2CModule.h"

I2CModule::I2CModule(I2C * i2c) {
  _i2c = i2c;
}

bool I2CModule::open(String address) {
  return open(strtol(address.c_str(), NULL, 16));
}

bool I2CModule::open(int address) {
  _address = address;
  return _i2c->ping(_address);
}

int I2CModule::readRegister(int registerAddress) {
  return _i2c->readRegister(_address, registerAddress);
}

void I2CModule::writeRegister(int registerAddress, int data) {
  _i2c->writeRegister(_address, registerAddress, data);
}

bool I2CModule::build(JsonObject json) {
  _name = "undefined";
  _type = "undefined";
  _chip = "undefined";
  _address = -1;

  if ( json.containsKey("name") ) {
    _name = json["name"].as<String>();
  } else {
    Serial.println("Needs a name");
    return false;
  }

  if ( json.containsKey("type") ) {
    _type = json["type"].as<String>();
  } else {
    Serial.println("Needs a type");
    return false;
  }

  if ( json.containsKey("address") ) {
    String hexStr = json["address"].as<String>();
    open(hexStr);
  } else {
    Serial.println("Needs an I2C Address");
    return false;
  }

  return true;
}

String I2CModule::getName() {
  return _name;
}

String I2CModule::getType() {
  return _type;
}