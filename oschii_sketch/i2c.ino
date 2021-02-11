const int SDA_PIN = 4;
const int SCL_PIN = 5;

const int I2C_ADDR = 0x20;
const int I2C_IODIR_ADDR[] = { 0x00, 0x01 };
const int I2C_RESISTANCE_ADDR[] = { 0x0C, 0x0D };
const int I2C_DATA_ADDR[] = { 0x12, 0x13 };

int i2CDir[] = { 0, 0 };
int i2CRes[] = { 0, 0 };
int i2CIn[] = { 0, 0 };
int i2COut[] = { 0, 0 };

//////////////
// I2C GPIO //
//////////////

void startI2CGpio() {
  Wire.begin(SDA_PIN, SCL_PIN);
}

void writeI2CGpio(int port, int pin, bool state) {
  if ( state ) {
    BIT_SET(i2COut[port], pin);
  } else {
    BIT_CLEAR(i2COut[port], pin);
  }
}

void flushI2CGpioCache() {
  sendI2C(I2C_ADDR, I2C_DATA_ADDR[0], i2COut[0]);
  sendI2C(I2C_ADDR, I2C_DATA_ADDR[1], i2COut[1]);
}

void setI2CGpioDirection(int port, int pin, bool output) {
  if ( output ) {
    BIT_CLEAR(i2CDir[port], pin);
  } else {
    BIT_SET(i2CDir[port], pin);
  }
  sendI2C(I2C_ADDR, I2C_IODIR_ADDR[port], i2CDir[port]);
}

void setI2CGpioResistance(int port, int pin, bool up) {
  if ( up ) {
    BIT_SET(i2CRes[port], pin);
  } else {
    BIT_CLEAR(i2CRes[port], pin);
  }
  sendI2C(I2C_ADDR, I2C_RESISTANCE_ADDR[port], i2CRes[port]);
}

bool readI2CGpio(int port, int pin) {
  return BIT_CHECK(i2CIn[port], pin);
}

void pullI2CGpio() {
  i2CIn[0] = requestI2C(I2C_ADDR, I2C_DATA_ADDR[0]);
  i2CIn[1] = requestI2C(I2C_ADDR, I2C_DATA_ADDR[1]);
}

void sendI2C(int deviceAddress, int registerAddress, int data) {
  Wire.beginTransmission(deviceAddress);
  Wire.write(registerAddress);
  Wire.write(data);
  Wire.endTransmission();
}

int requestI2C(int deviceAddress, int registerAddress) {
  Wire.beginTransmission(deviceAddress);
  Wire.write(registerAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, 1);
  return Wire.read();
}


/////////////
// I2C PWM //
/////////////

void startI2CPwm() {
  pwm.begin();
  pwm.setOscillatorFrequency(23000000);
  pwm.setPWMFreq(1000);
  Wire.setClock(400000);
}

