#include "I2CGpioModule.h"

int mode[] = { 0, 0 };
int resistance[] = { 0, 0 };
int in[] = { 0, 0 };
int out[] = { 0, 0 };

const int MODE_ADDR[] = { 0x00, 0x01 };
const int RESISTANCE_ADDR[] = { 0x0C, 0x0D };
const int DATA_ADDR[] = { 0x12, 0x13 };

void I2CGpioModule::setMode(int pin, bool output) {
  int port = portFor(pin);
  if ( port==1 ) pin -= 8;

  if ( output ) {
    BIT_CLEAR(mode[port], pin);
  } else {
    BIT_SET(mode[port], pin);
  }
  writeRegister(MODE_ADDR[port], mode[port]);
}

void I2CGpioModule::setResistance(int pin, bool up) {
  int port = portFor(pin);
  if ( port==1 ) pin -= 8;

  if ( up ) {
    BIT_SET(resistance[port], pin);
  } else {
    BIT_CLEAR(resistance[port], pin);
  }
  writeRegister(RESISTANCE_ADDR[port], resistance[port]);
}

bool I2CGpioModule::read(int pin) {
  int port = portFor(pin);
  if ( port==1 ) pin -= 8;

  pull(port);
  int value = BIT_CHECK(in[port], pin);
  return value;
}

void I2CGpioModule::write(int pin, bool value) {
  int port = portFor(pin);
  if ( port==1 ) pin -= 8;

  if ( value ) {
    BIT_SET(out[port], pin);
  } else {
    BIT_CLEAR(out[port], pin);
  }
  writeRegister(DATA_ADDR[port], out[port]);
}

int I2CGpioModule::portFor(int pin) {
  return pin > 7 ? 1 : 0;
}

void I2CGpioModule::pull(int port) {
  in[port] = readRegister(DATA_ADDR[port]);
}

bool I2CGpioModule::build(JsonObject json) {
  if ( !I2CModule::build(json) ) return false;

  if ( _type == "gpio" ) {
    _chip = "MCP23017";
  } else {
    setError("wtf?");
    return false;
  }

  return true;
}
