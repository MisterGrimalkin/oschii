#ifndef I2C_h
#define I2C_h

#include <Arduino.h>
#include <Wire.h>

class I2C {
  public:
    I2C() {};

    void start(int sdaPin, int sclPin);
    int readRegister(int deviceAddress, int registerAddress);
    void writeRegister(int deviceAddress, int registerAddress, int data);

    bool ping(int address);
    void scan();

  private:
    int _sdaPin, _sclPin;
};

#endif
