#include "I2CRack.h"

bool I2CRack::build(JsonObject json) {

  int sdaPin = -1;
  int sclPin = -1;

  _moduleIndex = 0;

  if ( json.containsKey("sdaPin") ) sdaPin = json["sdaPin"];
  if ( json.containsKey("sclPin") ) sclPin = json["sclPin"];

  if ( sdaPin < 0 || sclPin < 0 ) {
    setError("Must specify SDA and SCL pins");
    return false;
  }

  _i2c->start(sdaPin, sclPin);

  Serial.println("== Building I2C Modules");

  if ( json.containsKey("modules") ) {
    JsonArray modulesArray = json["modules"];
    for ( int i=0; i<modulesArray.size(); i++ ) {
      JsonObject moduleJson = modulesArray[i];
      if ( moduleJson.containsKey("type") ) {
        I2CModule * module;

        String type = moduleJson["type"].as<String>();
        if ( type=="gpio" ) {
          module = new I2CGpioModule(_i2c);

        } else if ( type=="pwm" ) {
          module = new I2CPwmModule(_i2c);

        } else {
          setError("Unknown I2C Module type '" + type + "'");
          return false;
        }

        if ( module->build(moduleJson) ) {
          _modules[_moduleIndex++] = module;
          Serial.println(" - " + module->toString());
        } else {
          setError(module->getError());
          return false;
        }
      } else {
        setError("No type specified");
        return false;
      }
    }
  }

  Serial.print("== Found:");
  Serial.print(_moduleIndex);
  Serial.println("\n");

  return true;
}

I2CModule * I2CRack::getModule(String name) {
  for ( int i=0; i<_moduleIndex; i++ ) {
    I2CModule * module = _modules[i];
    if ( module->getName() == name ) return module;
  }
  return NULL;
}

I2C * I2CRack::getI2C() {
  return _i2c;
}

String I2CRack::getError() {
  return _error;
}

void I2CRack::setError(String error) {
  _error = "I2C Rack: " + error;
}

I2CRack::I2CRack() {
  _i2c = new I2C();
  _moduleIndex = 0;
}

I2CRack::~I2CRack() {
  delete _i2c;
  for ( int i=0; i<_moduleIndex; i++ ) {
    I2CModule * module = _modules[i];
    delete module;
  }
}