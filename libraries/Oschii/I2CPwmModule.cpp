#include "I2CPwmModule.h"

bool I2CPwmModule::open(int address) {
  if ( !I2CModule::open(address) ) return false;

  _pwm = new Adafruit_PWMServoDriver(_address, Wire);
  _pwm->begin();
  _pwm->setOscillatorFrequency(23000000);
  _pwm->setPWMFreq(1000);
  Wire.setClock(400000);

  return true;
}

bool I2CPwmModule::build(JsonObject json) {
  if ( !I2CModule::build(json) ) return false;

  if ( _type == "pwm" ) {
    _chip = "PCA9685";
  } else {
    Serial.println("wtf?");
    return false;
  }

  return true;
}