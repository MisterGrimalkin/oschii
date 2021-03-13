#include "I2C.h"

void I2C::start(int sdaPin, int sclPin) {
  Wire.begin(sdaPin, sclPin);
  _sdaPin = sdaPin;
  _sclPin = sclPin;
  scan();
}

int I2C::readRegister(int deviceAddress, int registerAddress) {
  Wire.beginTransmission(deviceAddress);
  Wire.write(registerAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, 1);
  return Wire.read();
}

void I2C::writeRegister(int deviceAddress, int registerAddress, int data) {
  Wire.beginTransmission(deviceAddress);
  Wire.write(registerAddress);
  Wire.write(data);
  Wire.endTransmission();
}

bool I2C::ping(int address) {
  Wire.begin();
  Wire.beginTransmission(address);
  if ( Wire.endTransmission() == 0 ) {
    return true;
  } else {
    Serial.print("ERROR! I2C Module not found @ 0x");
    Serial.println(address, HEX);
    return false;
  }
}

void I2C::scan() {
  Serial.print("\n== Scanning I2C bus (SDA=");
  Serial.print(_sdaPin);
  Serial.print(" SCL=");
  Serial.print(_sclPin);
  Serial.println(")....");

  int count = 0;

  Wire.begin();
  for (int i=8; i<120; i++) {
      Wire.beginTransmission(i);
      if (Wire.endTransmission() == 0) {
      Serial.print(" - Found module @ 0x");
      Serial.println(i, HEX);
      count++;
    }
  }
  Serial.print("== Found: ");
  Serial.println(count);
  Serial.println();
}
