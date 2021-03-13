#include "I2CPwmModule.h"

I2CPwmModule::~I2CPwmModule() {
  if ( _pwm != NULL ) delete _pwm;
}

void I2CPwmModule::write(int pin, int value, bool invert) {
  if ( invert ) {
    _pwm->setPWM(pin, value, (value == 0 ? 4095 : 0));
  } else {
    _pwm->setPWM(pin, 0, value);
  }
}

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
    setError("wtf?");
    return false;
  }

  return true;
}