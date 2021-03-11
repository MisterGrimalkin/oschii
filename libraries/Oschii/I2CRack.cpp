#include "I2CRack.h"

bool I2CRack::build(JsonObject json) {
  Serial.println("\n== Building I2C Modules");

  _i2c = new I2C();

  int sdaPin = -1;
  int sclPin = -1;

  _moduleIndex = 0;

  if ( json.containsKey("sdaPin") ) sdaPin = json["sdaPin"];
  if ( json.containsKey("sclPin") ) sclPin = json["sclPin"];

  if ( sdaPin < 0 || sclPin < 0 ) {
    Serial.println("Must specify SDA and SCL pins");
    return false;
  }

  Serial.print("   (SDA=");
  Serial.print(sdaPin);
  Serial.print(" SCL=");
  Serial.print(sclPin);
  Serial.println(")");

  _i2c->start(sdaPin, sclPin);

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
          Serial.println("Unknown I2C Module type");
          return false;
        }
        if ( module->build(moduleJson) ) {
          _modules[_moduleIndex++] = module;
        } else {
          Serial.println("failed to build i2c fucking module");
          return false;
        }
      } else {
        Serial.println("No type specified");
        return false;
      }
    }
  }

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