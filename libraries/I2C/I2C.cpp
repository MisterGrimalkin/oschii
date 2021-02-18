#include "I2C.h"

#include <Wire.h>

I2C::I2C() {}

void I2C::start_gpio(int sda_pin, int scl_pin) {
  Wire.begin(sda_pin, scl_pin);
}

int I2C::read_gpio(int device_address, int register_address) {
  Wire.beginTransmission(device_address);
  Wire.write(register_address);
  Wire.endTransmission();
  Wire.requestFrom(device_address, 1);
  return Wire.read();
}

void I2C::write_gpio(int device_address, int register_address, int data) {
  Wire.beginTransmission(device_address);
  Wire.write(register_address);
  Wire.write(data);
  Wire.endTransmission();
}
