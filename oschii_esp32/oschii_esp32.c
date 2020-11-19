#include <Adafruit_PWMServoDriver.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoOSC.h>
#include <ESPAsyncWebServer.h>

#include <ETH.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <WiFi.h>
#include <Wire.h>

#include "SPIFFS.h"

#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))
#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))

bool reboot = false;

bool verbose = true;

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

const int I2C_PWM_ADDR = 0x40;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(I2C_PWM_ADDR, Wire);

String configPayload = "";

const int JSON_SIZE_LIMIT = 8192;

StaticJsonDocument<JSON_SIZE_LIMIT> doc;

const String CONFIG_OK = "Got it. Sounds fun!\n";

Preferences prefs;

String name     = "";
String ssid     = "";
String password = "";

const String  DEFAULT_NAME = "Oschii";
const char *  HTTP_CONFIG_ENDPOINT = "/config";
const char *  HTTP_NAME_ENDPOINT = "/name";
const int     OSC_PING_PORT = 3333;
const String  OSC_PING_ADDR = "/hello_oschii";
const int     OSC_PING_RESPONSE_PORT = 3333;
const String  OSC_PING_RESPONSE_ADDR = "/i_am_oschii";

const bool DIR_OUT = true;
const bool DIR_IN = false;

const bool PULL_UP = true;
const bool PULL_DOWN = false;

static bool enableEthernet = false;

AsyncWebServer server(80);
bool connected = false;
static String connectionType = "";

bool oschiiGO = false;

int pwmChannelCount = 0;


/////////////
// Structs //
/////////////

const int DEVICES_LIMIT = 64;
const int INPUTS_LIMIT = 128;
const int CONTROLLERS_LIMIT = 128;
const int RECEIVERS_LIMIT = 128;

struct Device {
  String name;
  String ip;
};
Device devices[DEVICES_LIMIT];
int deviceCount = 0;

struct Sensor {
  String type;
  int value;
  bool changed;
  int lastChangedAt;
  int controllerStartIndex;
  int controllerCount;
  int receiverStartIndex;
  int receiverCount;
};
Sensor sensors[INPUTS_LIMIT];
int sensorCount = 0;

struct GpioSensor {
  int i2cPort; // -1 for none
  int pin;
  String resistor;
  int onValue;
  int offValue; // -1 for none
  bool invert;
  int bounceFilter;
  bool lastPolledState;
};
GpioSensor gpioSensors[INPUTS_LIMIT];

struct Receiver {
  String ip;
  int oscPort;
  String oscAddress;
  String httpMethod;
  String httpPath;
};
Receiver receivers[RECEIVERS_LIMIT];
int receiverCount = 0;

struct Controller {
  String type;
};
Controller controllers[CONTROLLERS_LIMIT];
int controllerCount = 0;

struct GpioController {
  int i2cPort;
  int pin;
  bool onState;
  int valueThreshold;
  bool invert;
  bool follow;
  bool toggle;
  int pulseLength;
  bool currentState;
};
GpioController gpioControllers[CONTROLLERS_LIMIT];

struct PwmController {
  int i2cPort;
  int pin;
  int channel;
  String valueTransform;
  bool invert;
};
PwmController pwmControllers[CONTROLLERS_LIMIT];

struct Pulser {
  int i2cPort;
  int pin;
  bool targetState;
  int duration;
  int startedAt;
};
Pulser pulsers[CONTROLLERS_LIMIT];
int pulserCount = 0;

//////////
// Main //
//////////

static String configToLoad = "";

void setup() {
  name = readFromStorage("name");
  if ( name == "" ) {
    name = DEFAULT_NAME;
  }
  ssid = readFromStorage("ssid");
  password = readFromStorage("password");

  startSerial();

  String enableEthernetStr = readFromStorage("enableEthernet");
  if ( enableEthernetStr == "yes" ) {
    enableEthernet = true;
  }

  startNetwork();
  startI2CGpio();
  startI2CPwm();

  const String config = readFile("/config.json");
  if ( config != "" ) {
    parseJson(config);
  }

  String cloudIp = readFromStorage("CloudIP");
  if ( cloudIp != "" ) {
    Serial.println("Pinging Cloud....");
    OscWiFi.send(
      cloudIp,
      OSC_PING_RESPONSE_PORT,
      OSC_PING_RESPONSE_ADDR,
      ("Hello my name is: " + name).c_str()
    );
  }

  oschiiGO = true;
}

void loop() {
  if ( oschiiGO ) {
    loopSerial();
    loopOsc();
    scanPulsers();
    scanSensors();
//    delay(10);
    if ( reboot ) {
      Serial.println("\n!!!!! R E B O O T I N G !!!!!\n");
      delay(1000);
      ESP.restart();
    }
  }
}


/////////////////////
// Sensor Builders //
/////////////////////

String errorMessage = "";

int buildGpioSensor(int index, JsonObject inputJson) {
  pullI2CGpio();

  errorMessage = "";

  int value = -1;

  int pin = -1;
  int i2cPort = -1;
  String resistor = "off";
  int onValue = 1;
  int offValue = -1;
  bool invert = false;
  int bounceFilter = 100;

  if ( inputJson.containsKey("pin") )          pin =          inputJson["pin"];
  if ( inputJson.containsKey("i2cPort") )      i2cPort =      inputJson["i2cPort"];
  if ( inputJson.containsKey("resistor") )     resistor =     inputJson["resistor"].as<String>();
  if ( inputJson.containsKey("onValue") )      onValue =      inputJson["onValue"];
  if ( inputJson.containsKey("offValue") )     offValue =     inputJson["offValue"];
  if ( inputJson.containsKey("invert") )       invert =       inputJson["invert"];
  if ( inputJson.containsKey("bounceFilter") ) bounceFilter = inputJson["bounceFilter"];

  if ( i2cPort < 0 ) {
    if ( pin < 0 ) {
      errorMessage = "ERROR! Input " + String(index) + ": No GPIO pin specified";
      return value;
    } else if ( pin == 0 && resistor != "up" ) {
      errorMessage = "ERROR! Input " + String(index) + ": GPIO 0 is fixed to PULL-UP";
      return value;
    } else if ( pin == 1 || pin == 3 || ( pin >= 6 && pin <= 12 ) ) {
      errorMessage = "ERROR! Input " + String(index) + ": GPIOs 1, 3, 6-12 not recommended for use an inputs";
      return value;
    }
  }

  if ( i2cPort > 1 ) {
    errorMessage = "ERROR! Input " + String(index) + ": If specified, I2C Port must be 0 or 1";
    return value;
  }

  int resistance;
  if ( resistor == "off" ) {
    resistance = INPUT;
  } else if ( resistor == "up" ) {
    resistance = INPUT_PULLUP;
    invert = !invert;
  } else if ( resistor == "down" ) {
    if ( i2cPort >= 0 ) {
      errorMessage = "ERROR! Input " + String(index) + ": I2C Expander does not support 'down' resistor mode";
      return value;
    }
    resistance = INPUT_PULLDOWN;
  } else {
    errorMessage = "ERROR! Input " + String(index) + ": Invalid resistor mode '" + resistor + "'";
    return value;
  }

  if ( i2cPort < 0 ) {
    pinMode(pin, resistance);
    value = digitalRead(pin);
  } else {
    setI2CGpioResistance(i2cPort, pin, resistance == INPUT_PULLUP);
    setI2CGpioDirection(i2cPort, pin, false);
    value = readI2CGpio(i2cPort, pin);
  }

  GpioSensor gpioSensor = {
    i2cPort, pin, resistor, onValue, offValue, invert, bounceFilter, offValue
  };
  gpioSensors[index] = gpioSensor;

  if ( verbose ) printGpioSensor(gpioSensor);

  return value;
}

int buildGpioController(int index, JsonObject controllerJson) {
  errorMessage = "";

  int i2cPort = -1;
  int pin = -1;
  bool onState = true;
  int valueThreshold = 1;
  bool invert = false;
  bool follow = true;
  bool toggle = false;
  int pulseLength = -1;

  if ( controllerJson.containsKey("pin") )            pin =                 controllerJson["pin"];
  if ( controllerJson.containsKey("i2cPort") )        i2cPort =             controllerJson["i2cPort"];
  if ( controllerJson.containsKey("onState") )        onState =             controllerJson["onState"];
  if ( controllerJson.containsKey("valueThreshold") ) valueThreshold =      controllerJson["valueThreshold"];
  if ( controllerJson.containsKey("invert") )         invert =              controllerJson["invert"];
  if ( controllerJson.containsKey("follow") )         follow =              controllerJson["follow"];
  if ( controllerJson.containsKey("toggle") )         toggle =              controllerJson["toggle"];
  if ( controllerJson.containsKey("pulseLength") && !toggle ) pulseLength = controllerJson["pulseLength"];

  if ( pin < 0 ) {
    errorMessage = "ERROR! Controller " + String(index) + ": No GPIO pin specified";
    return -1;
  }

  if ( i2cPort > 1 ) {
    errorMessage = "ERROR! Input " + String(index) + ": If specified, I2C Port must be 0 or 1";
    return -1;
  }

  if ( i2cPort < 0 ) {
    pinMode(pin, OUTPUT);
  } else {
    setI2CGpioDirection(i2cPort, pin, true);
  }

  bool currentState = false;
  if ( controllerJson.containsKey("initialState") ) {
    currentState = controllerJson["initialState"];
    if ( i2cPort < 0 ) {
      digitalWrite(pin, currentState);
    } else {
      writeI2CGpio(i2cPort, pin, currentState);
    }
    if ( toggle ) {
      onState = !currentState;
    }
  }

  GpioController gpioController = {
    i2cPort, pin, onState, valueThreshold, invert, follow, toggle, pulseLength, currentState
  };
  gpioControllers[index] = gpioController;

  if ( verbose ) printGpioController(gpioController);

  return 0;
}

int buildPwmController(int index, JsonObject controllerJson) {
  errorMessage = "";

  int i2cPort = -1;
  int pin = -1;
  String valueTransform = "none";
  int initialValue = 0;
  bool invert = false;

  int channel = pwmChannelCount++;

  if ( controllerJson.containsKey("i2cPort") )          i2cPort =         controllerJson["i2cPort"];
  if ( controllerJson.containsKey("pin") )              pin =             controllerJson["pin"];
  if ( controllerJson.containsKey("valueTransform") )   valueTransform =  controllerJson["valueTransform"].as<String>();
  if ( controllerJson.containsKey("initialValue") )     initialValue =    controllerJson["initialValue"];
  if ( controllerJson.containsKey("invert") )           invert =          controllerJson["invert"];

  if ( pin < 0 ) {
    errorMessage = "ERROR! Controller " + String(index) + ": No GPIO pin specified";
    return -1;
  }

  if ( i2cPort > 1 ) {
    errorMessage = "ERROR! Input " + String(index) + ": If specified, I2C Port must be 0 or 1";
    return -1;
  }

  if ( i2cPort < 0 ) {
    ledcAttachPin(pin, channel);
    ledcSetup(channel, 4000, 8);
  } else {
    if ( invert ) {
      pwm.setPWM(pin, 0, 4095);
    } else {
      pwm.setPWM(pin, 0, 0);
    }
  }

  PwmController pwmController = {
    i2cPort, pin, channel, valueTransform, invert
  };
  pwmControllers[index] = pwmController;

  if ( verbose ) printPwmController(pwmController);

  return 0;

}

bool buildReceiver(int index, JsonObject receiverJson) {
  String ip = "";
  int oscPort = 3333;
  String oscAddress = "";
  String httpMethod = "post";
  String httpPath = "";

  if ( receiverJson.containsKey("ip") ) {
    ip = receiverJson["ip"].as<String>();
  } else if ( receiverJson.containsKey("device") ) {
    ip = getDeviceIp(receiverJson["device"].as<String>());
  }

  if ( receiverJson.containsKey("oscAddress") ) {
    oscAddress = receiverJson["oscAddress"].as<String>();
    if ( receiverJson.containsKey("oscPort") ) oscPort = receiverJson["oscPort"];
  } else if ( receiverJson.containsKey("httpPath")) {
    oscPort = -1;
    httpPath = receiverJson["httpPath"].as<String>();
    if ( receiverJson.containsKey("httpMethod") ) httpMethod = receiverJson["httpMethod"].as<String>();
  } else {
    return false;
  }

  Receiver receiver = {
    ip, oscPort, oscAddress, httpMethod, httpPath
  };
  receivers[index] = receiver;

  if ( verbose ) printReceiver(receiver);

  return true;
}

void printGpioSensor(GpioSensor sensor) {
  Serial.println("   + Input:GPIO");
  Serial.print  ("     [pin:");
  if ( sensor.i2cPort >= 0 ) {
    Serial.print(sensor.i2cPort);
    Serial.print("/");
  }
  Serial.print(sensor.pin);
  Serial.print(" resistor:");
  Serial.print(sensor.resistor);
  Serial.print(" on:");
  Serial.print(sensor.onValue);
  Serial.print(" off:");
  if ( sensor.offValue == -1 ) {
    Serial.print("(none)");
  } else {
    Serial.print(sensor.offValue);
  }
  Serial.print(" invert:");
  Serial.print(sensor.invert);
  Serial.print(" bounce:");
  Serial.print(sensor.bounceFilter);
  Serial.println("]");
}

void printGpioController(GpioController controller) {
  Serial.println("     + Controller:GPIO");
  Serial.print  ("       [pin:");
  if ( controller.i2cPort >= 0 ) {
    Serial.print(controller.i2cPort);
    Serial.print("/");
  }
  Serial.print(controller.pin);
  Serial.print(" on:");
  Serial.print(controller.onState);
  Serial.print(" threshold:");
  Serial.print(controller.valueThreshold);
  Serial.print(" invert:");
  Serial.print(controller.invert);
  Serial.print(" follow:");
  Serial.print(controller.follow);
  Serial.print(" toggle:");
  Serial.print(controller.toggle);
  Serial.print(" pulse:");
  Serial.print(controller.pulseLength);
  Serial.println("]");
}

void printPwmController(PwmController controller) {
  Serial.println("     + Controller:PWM");
  Serial.print  ("       [pin:");
  if ( controller.i2cPort >= 0 ) {
    Serial.print(controller.i2cPort);
    Serial.print("/");
  }
  Serial.print(controller.pin);
  Serial.print(" transform:");
  Serial.print(controller.valueTransform);
  Serial.print(" invert:");
  Serial.print(controller.invert);
  Serial.println("]");
}

void printReceiver(Receiver receiver) {
  Serial.print("     + Receiver:");
  if ( receiver.oscPort > 0 ) {
    Serial.println("OSC");
    Serial.print("       [ip:");
    Serial.print(receiver.ip);
    Serial.print(" port:");
    Serial.print(receiver.oscPort);
    Serial.print(" address:");
    Serial.print(receiver.oscAddress);
    Serial.println("]");
  } else {
    Serial.println("HTTP");
    Serial.print("       [ip:");
    Serial.print(receiver.ip);
    Serial.print(" method:");
    Serial.print(receiver.httpMethod);
    Serial.print(" path:");
    Serial.print(receiver.httpPath);
    Serial.println("]");
  }
}

String getDeviceIp(String name) {
  int i;
  for ( i = 0; i < deviceCount; i++ ) {
    Device device = devices[i];
    if ( device.name == name ) {
      return device.ip;
    }
  }
  return "";
}

void scanPulsers() {
  int i = 0;
  int j = 0;
  Pulser newPulsers[INPUTS_LIMIT];
  for ( i=0; i<pulserCount; i++ ) {
    Pulser pulser = pulsers[i];
    if ( millis() - pulser.startedAt >= pulser.duration ) {
        if ( pulser.i2cPort == -1 ) {
          digitalWrite(pulser.pin, pulser.targetState);
        } else {
          writeI2CGpio(pulser.i2cPort, pulser.pin, pulser.targetState);
        }
    } else {
      pulsers[j++] = pulser;
    }
  }

  flushI2CGpioCache();

  pulserCount = j;
}

void scanSensors() {
  pullI2CGpio();

  int i;
  for ( i=0; i<sensorCount; i++ ) {
    Sensor * sensor = readSensor(i);
    if ( sensor->changed ) {
      int value = sensor->value;
      Serial.print("[#");
      Serial.print(i);
      Serial.print("] ---> ");
      Serial.println(value);

      int c;
      for ( c = sensor->controllerStartIndex; c < (sensor->controllerStartIndex + sensor->controllerCount); c++) {
        Controller * controller = &controllers[c];

        if ( controller->type == "gpio" ) {
          GpioController * gpioController = &gpioControllers[c];

          bool valueIsOn = ( !gpioController->invert && value >= gpioController->valueThreshold )
                        || ( gpioController->invert && value < gpioController->valueThreshold );

          if ( gpioController->i2cPort < 0 ) {
            if ( valueIsOn ) {
              digitalWrite(gpioController->pin, gpioController->onState);
            } else if ( gpioController->follow ) {
              digitalWrite(gpioController->pin, !gpioController->onState);
            }
          } else {
            if ( valueIsOn ) {
              writeI2CGpio(gpioController->i2cPort, gpioController->pin, gpioController->onState);
            } else if ( gpioController->follow ) {
              writeI2CGpio(gpioController->i2cPort, gpioController->pin, !gpioController->onState);
            }
          }

          if ( gpioController->pulseLength > 0 ) {
            pulsers[pulserCount++] = {
              gpioController->i2cPort, gpioController->pin, !gpioController->onState, gpioController->pulseLength, millis()
            };
          }

          if ( gpioController->toggle ) {
            gpioController->onState = !gpioController->onState;
          }

        } else if ( controller->type == "pwm" ) {
          PwmController * pwmController = &pwmControllers[c];

          int max = 255;
          if ( pwmController->i2cPort >= 0 ) {
            max = 4095;
          }

          if ( pwmController->valueTransform == "binary" ) {
            if ( value > 0 ) value = max;
          } else if ( pwmController->valueTransform == "percentage" ) {
            if ( value >= 100 ) {
              value = max;
            } else if ( value <= 0 ) {
              value = 0;
            } else {
              value = (value / 100.0) * max;
            }

          } else if ( pwmController->valueTransform == "8bit" ) {
            if ( value >= 255 ) {
              value = max;
            } else if ( value <= 0 ) {
              value = 0;
            } else {
              value = (value / 255.0) * max;
            }
          }

          if ( pwmController->i2cPort < 0 ) {
            ledcWrite(pwmController->channel, value);
          } else {
            if ( pwmController->invert ) {
              pwm.setPWM(pwmController->pin, value, (value == 0 ? 4095 : 0));
            } else {
              pwm.setPWM(pwmController->pin, 0, value);
            }
          }
        }
      }

      flushI2CGpioCache();

      int r;
      for ( r = sensor->receiverStartIndex; r < (sensor->receiverStartIndex + sensor->receiverCount); r++ ) {
        Receiver * receiver = &receivers[r];
        if ( receiver->oscPort > 0 ) {
          sendOsc(
            receiver->ip,
            receiver->oscPort,
            receiver->oscAddress,
            value
          );
        } else {
          String url = "http://" + receiver->ip + receiver->httpPath;
          sendHttp(
            receiver->httpMethod,
            url,
            String(value)
          );
        }
      }

      sensor->changed = false;
    }
  }
}

Sensor * readSensor(int sensorIndex) {
  return readSensor(sensorIndex, &sensors[sensorIndex]);
}

Sensor * readSensor(int sensorIndex, Sensor * sensor) {
  if ( sensor->type == "gpio" ) {
    GpioSensor * gpioSensor = &gpioSensors[sensorIndex];

    if ( (millis() - sensor->lastChangedAt) < gpioSensor->bounceFilter ) {
      return sensor;
    }

    int state = -1;
    if ( gpioSensor->i2cPort == -1 ) {
      state = digitalRead(gpioSensor->pin);
    } else {
      state = readI2CGpio(gpioSensor->i2cPort, gpioSensor->pin);
    }

    if ( state != gpioSensor->lastPolledState ) {
      if ( (state && !gpioSensor->invert) || (!state && gpioSensor->invert) ) {
        sensor->value = gpioSensor->onValue;
        sensor->changed = true;
      } else {
        sensor->value = gpioSensor->offValue;
        sensor->changed = gpioSensor->offValue >= 0;
      }
      sensor->lastChangedAt = millis();
    }

    gpioSensor->lastPolledState = state;
  }
  return sensor;
}

int buildControllers(JsonArray controllersJson) {
  int count = 0;
  int c;
  for ( c = 0; c < (CONTROLLERS_LIMIT - controllerCount); c++ ) {
    JsonObject controllerJson = controllersJson[c];

    if ( controllerJson.containsKey("type") ) {
      String controllerType = controllerJson["type"];

      if ( controllerType == "gpio" ) {
        int result = buildGpioController(controllerCount, controllerJson);
        if ( result < 0 ) {
          return -1;
        }
      } else if ( controllerType == "pwm" ) {
        int result = buildPwmController(controllerCount, controllerJson);
        if ( result < 0 ) {
          return -1;
        }
      } else {
        errorMessage = "Unknown Controller type: " + controllerType;
        return -1;
      }

      Controller cont = { controllerType };
      controllers[controllerCount++] = cont;
      count++;
    }
  }

  flushI2CGpioCache();

  return count;
}

int buildReceivers(JsonArray receiversJson) {
  int count = 0;
  int r = 0;
  for ( r = 0; r < (RECEIVERS_LIMIT - receiverCount); r++ ) {
    JsonObject receiverJson = receiversJson[r];
    if ( buildReceiver(receiverCount, receiverJson) ) {
      receiverCount++;
      count++;
    }
  }
  return count;
}

String parseJson(String input) {
  if ( verbose ) {
    Serial.println("\n~~ Oschii is receiving new configuration ~~");
  }
  oschiiGO = false;

  deviceCount = 0;
  sensorCount = 0;

  const char * json = input.c_str();
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    String reply = "ERROR! Parsing JSON: " + String(error.c_str());
    if ( verbose ) {
      Serial.println(reply);
      Serial.println("\n~~ Oschii could not comply :( ~~\n");
    }
    oschiiGO = true;

    return reply;
  }

  int i;

  ////////// DEVICES //////////

  if ( doc.containsKey("devices") ) {
    if ( verbose ) Serial.println("\n-- Parsing Devices");

    JsonArray devicesJson = doc["devices"];
    for ( i = 0; i < DEVICES_LIMIT; i++ ) {
      JsonObject deviceJson = devicesJson[i];
      if ( deviceJson.containsKey("name") ) {
        if ( deviceJson.containsKey("ip") ) {
          String name = deviceJson["name"];
          String ip = deviceJson["ip"];
          if ( verbose ) {
            Serial.print("   Device ");
            Serial.print(name);
            Serial.print(" at ");
            Serial.println(ip);
          }
          devices[deviceCount++] = { deviceJson["name"], deviceJson["ip"] };
        } else {
          String name = deviceJson["name"];
          return "ERROR! Missing IP for Device '" + name + "'";
        }
      }
    }
    if ( verbose ) {
      Serial.print("-- FOUND: ");
      Serial.println(deviceCount);
    }
  } else {
    if ( verbose ) Serial.println("\n-- No Devices");
  }

  ///////// INPUTS //////////

//  pullI2CGpio();

  if (doc.containsKey("inputs")) {
    if ( verbose ) Serial.println("\n-- Parsing INPUTS");

    JsonArray inputsJson = doc["inputs"];
    for ( i = 0; i < INPUTS_LIMIT; i++ ) {
      JsonObject inputJson = inputsJson[i];

      if ( inputJson.containsKey("type") ) {

        /* Sensor */

        String sensorType = inputJson["type"];
        int value = -1;
        if ( sensorType == "gpio" ) {
          value = buildGpioSensor(sensorCount, inputJson);
          if ( value < 0 ) {
            return errorMessage;
          }
        } else {
          return "ERROR! Input " + String(sensorCount) + ": Unknown Input type '" + sensorType + "'";
        }

        /* Controllers */

        int inputControllerStartIndex = controllerCount;
        int inputControllerCount = 0;

        if ( inputJson.containsKey("controllers") ) {
          inputControllerCount = buildControllers(inputJson["controllers"]);
          if ( inputControllerCount < 0 ) {
            return errorMessage;
          }
        }

        /* Receivers */

        int inputReceiverStartIndex = receiverCount;
        int inputReceiverCount = 0;

        if ( inputJson.containsKey("receivers") ) {
          inputReceiverCount = buildReceivers(inputJson["receivers"]);
          if ( inputReceiverCount < 0 ) {
            return errorMessage;
          }
        }

        /* Save Input */

        Sensor sensor = {
          sensorType, value, false, -1,
          inputControllerStartIndex, inputControllerCount,
          inputReceiverStartIndex, inputReceiverCount
        };
        sensors[sensorCount++] = sensor;
      }
    }
    if ( verbose ) {
      Serial.print("-- FOUND: ");
      Serial.println(sensorCount);
    }
  } else {
    if ( verbose ) Serial.println("\n-- No Inputs");
  }

  int inputCount = sensorCount;

  ///////// OUTPUTS //////////

  if ( doc.containsKey("outputs") ) {
    if ( verbose ) Serial.println("\n-- Parsing OUTPUTS");

    JsonArray outputsJson = doc["outputs"];

    for ( i=0; i<(INPUTS_LIMIT - sensorCount); i++ ) {
      JsonObject outputJson = outputsJson[i];

      if ( outputJson.containsKey("oscAddress") ) {
        createOscTrigger(outputJson);
      } else if ( outputJson.containsKey("httpPath") ) {
        createHttpTrigger(outputJson);
      } else {
        continue;
      }

      /* Controllers */

      int outputControllerStartIndex = controllerCount;
      int outputControllerCount = 0;

      if ( outputJson.containsKey("controllers") ) {
        outputControllerCount = buildControllers(outputJson["controllers"]);
        if ( outputControllerCount < 0 ) {
          return errorMessage;
        }
      }

      /* Receivers */

      int outputReceiverStartIndex = receiverCount;
      int outputReceiverCount = 0;

      if ( outputJson.containsKey("receivers") ) {
        outputReceiverCount = buildReceivers(outputJson["receivers"]);
        if ( outputReceiverCount < 0 ) {
          return errorMessage;
        }
      }

      /* Save Output */

      Sensor sensor = {
        "output", -1, false, -1,
        outputControllerStartIndex, outputControllerCount,
        outputReceiverStartIndex, outputReceiverCount
      };
      sensors[sensorCount++] = sensor;
    }
    if ( verbose ) {
      Serial.print("-- FOUND: ");
      Serial.println(sensorCount - inputCount);
    }
  } else {
    if ( verbose ) Serial.println("\n-- No Outputs");
  }

  if (!writeFile("/config.json", input)) {
    if ( verbose ) {
      Serial.println("\n~~ Oschii could not write config :( ~~\n");
    }
  }

  if ( verbose ) {
    Serial.println("\n~~ Oschii is ready :D ~~\n");
  }
  oschiiGO = true;

  return CONFIG_OK;
}


void createHttpTrigger(JsonObject outputJson) {
  if ( verbose ) {
    Serial.println("   + Output:HTTP");
  }
  const char * path = outputJson["httpPath"].as<char*>();

  WebRequestMethod method;
  String methodStr = "post";
  if ( outputJson.containsKey("httpMethod") ) outputJson["httpMethod"];
  if ( methodStr == "post" ) {
    method = HTTP_POST;
  } else if ( methodStr == "put" ) {
    method = HTTP_PUT;
  } else if ( methodStr == "delete" ) {
    method = HTTP_DELETE;
  } else {
    method = HTTP_GET;
  }
  const int sensorNo = sensorCount;
  server.on(path, method, [sensorNo](AsyncWebServerRequest * request) {
    String valueStr = request->getParam("body", true)->value();
    int value = valueStr.toInt();
    if ( verbose ) {
      Serial.print(value);
      Serial.println(" <--- HTTP");
    }
    Sensor * sensor= &sensors[sensorNo];
    sensor->value = value;
    sensor->changed = true;
    request->send(200, "text/plain", "OK");
  }, NULL, [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {});

  if ( verbose ) {
    Serial.print("     [method:");
    Serial.print(methodStr);
    Serial.print(" path:");
    Serial.print(path);
    Serial.println("]");
  }
}

void createOscTrigger(JsonObject outputJson) {
  if ( verbose ) {
    Serial.println("   + Output:OSC");
  }
  String address = outputJson["oscAddress"];

  int port = 3333;
  if ( outputJson.containsKey("oscPort") ) port = outputJson["oscPort"];

  const int sensorNo = sensorCount;
  OscWiFi.subscribe(port, address,
    [sensorNo](const OscMessage & m) {
      int value = m.arg<int>(0);
      if ( verbose ) {
        Serial.print(value);
        Serial.println(" <--- OSC");
      }
      Sensor * sensor = &sensors[sensorNo];
      sensor->value = value;
      sensor->changed = true;
    }
  );

  if ( verbose ) {
    Serial.print("     [port:");
    Serial.print(port);
    Serial.print(" address:");
    Serial.print(address);
    Serial.println("]");
  }
}


////////////
// EEPROM //
////////////

void writeToStorage(String key, String value) {
  prefs.begin("oschii", false);
  prefs.putString(key.c_str(), value);
  prefs.end();
}

String readFromStorage(String key) {
  prefs.begin("oschii", true);
  String value = prefs.getString(key.c_str());
  prefs.end();
  return value;
}


////////////
// SPIFFS //
////////////

bool writeFile(String filename, String content) {
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return false;
  }

  File file = SPIFFS.open(filename, FILE_WRITE);

  if (!file) {
    Serial.println("There was an error opening the file for writing");
    return false;
  }
  if (!file.print(content)) {
    Serial.println("ERROR! File write failed");
  }

  file.close();

  return true;
}

String readFile(String filename) {
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return "";
  }

  File file = SPIFFS.open(filename);

  if (!file) {
    Serial.println("There was an error opening the file for reading");
    return "";
  }

  String result = "";

  while (file.available()) {
    result += file.readString();
  }

  file.close();

  return result;
}

/////////
// API //
/////////

void createApi() {
  createOscPing();
  createHttpApi();
}

void createOscPing() {
  OscWiFi.subscribe(OSC_PING_PORT, OSC_PING_ADDR,
  [](const OscMessage & m) {
    Serial.print("Received ping from ");
    Serial.println(m.remoteIP());
    writeToStorage("CloudIP", m.remoteIP());
    OscWiFi.send(
      m.remoteIP(),
      OSC_PING_RESPONSE_PORT,
      OSC_PING_RESPONSE_ADDR,
      ("Hello my name is: " + name).c_str()
    );
  });
}

void createHttpApi() {
  server.on(HTTP_NAME_ENDPOINT, HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", name);
  });

  server.on(HTTP_NAME_ENDPOINT, HTTP_POST, [](AsyncWebServerRequest * request) {
    name = request->getParam("body", true)->value();
    writeToStorage("name", name);
    String reply = "Name is now " + name + "\n";
    request->send(200, "text/plain", reply);
  }, NULL, [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {});

  server.on(HTTP_CONFIG_ENDPOINT, HTTP_GET, [](AsyncWebServerRequest * request) {
    String config = readFile("/config.json");
    request->send(200, "text/plain", config);
  });

  server.on(HTTP_CONFIG_ENDPOINT, HTTP_POST, [](AsyncWebServerRequest * request) {
    String reply = parseJson(request->getParam("body", true)->value());
    int ok = (reply == CONFIG_OK);
    request->send(ok ? 200 : 400, "text/plain", reply);
    reboot = true;
  }, NULL, [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {});
}


////////////
// Serial //
////////////

void startSerial() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("  ╔═╗┌─┐┌─┐┬ ┬┬┬");
  Serial.println("  ║ ║└─┐│  ├─┤││");
  Serial.println("  ╚═╝└─┘└─┘┴ ┴┴┴");
  Serial.print  ("  ");
  Serial.println(name);
  Serial.println();
}

void processSerialInput(String input) {
  if ( input == "poke" ) {
    Serial.println("Tickles!");

  } else if ( input == "name" ) {
    Serial.println(name);

  } else if ( input == "ssid" ) {
    Serial.println(ssid);

  } else if ( input == "set name" ) {
    name = promptSerial("Ready for name");
    writeToStorage("name", name);
    Serial.print("Name is now ");
    Serial.println(name);

  } else if ( input == "config" ) {
    Serial.println(readFile("/config.json"));

  } else if ( input == "set config" ) {
    String config = promptSerial("Ready for config");
    Serial.println(parseJson(config));;

  } else if ( input == "start wifi" ) {
    enableEthernet = false;
    writeToStorage("enableEthernet", "no");
    ssid = promptSerial("Ready for ssid");
    writeToStorage("ssid", ssid);
    password = promptSerial("Ready for password");
    writeToStorage("password", password);
    startWiFi();

  } else if ( input == "start ethernet" ) {
    stopWiFi();
    startEthernet();
    enableEthernet = true;
    writeToStorage("enableEthernet", "yes");

  } else if ( input == "stop ethernet" ) {
    stopWiFi();
    enableEthernet = false;
    writeToStorage("enableEthernet", "no");
    Serial.println("Restart ESP now");

  } else if ( input == "ip" ) {
    if ( connected ) {
      String ip = WiFi.localIP().toString();
      if ( connectionType == "Ethernet" ) {
        ip = ETH.localIP().toString();
      }
      Serial.print(ip);
      Serial.print(" (");
      Serial.print(connectionType);
      Serial.println(")");
    } else {
      Serial.println("DISCONNECTED");
    }
  }
}

void loopSerial() {
  String input = readSerial();
  if ( input != "" ) {
    processSerialInput(input);
  }
}

String promptSerial(String prompt) {
  Serial.println(prompt);
  while (!Serial.available());
  return readSerial();
}

String readSerial() {
  if ( Serial.available() ) {
    return Serial.readString();
  }
  return "";
}


//////////
// WiFi //
//////////

const int WIFI_TIMEOUT = 5000;
const int ETHERNET_TIMEOUT = 120000;

void startNetwork() {
  if ( enableEthernet ) {
    startEthernet();
  } else {
    startWiFi();
  }
}

void WiFiEvent(WiFiEvent_t event)
{
  switch (event) {
    case SYSTEM_EVENT_ETH_START:
      Serial.print("<ETH Started>");
      ETH.setHostname(name.c_str());
      stopWiFi();
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.print("<ETH Connected>");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.print("\nConnected to Ethernet on ");
      Serial.println(ETH.localIP());
      Serial.print("  MAC: ");
      Serial.print(ETH.macAddress());
      if (ETH.fullDuplex()) {
        Serial.print(" FULL_DUPLEX");
      }
      Serial.print(" ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      connected = true;
      connectionType = "Ethernet";
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("<ETH Disconnected>");
      connected = false;
      startWiFi();
      break;
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("<ETH Stopped>");
      connected = false;
      break;
    default:
      break;
  }
}

void startEthernet() {
  WiFi.onEvent(WiFiEvent);
  ETH.begin();
  Serial.print("Accessing Ethernet");
  int started = millis();
  while (!connected) {
    Serial.print(".");
    delay(500);
    if ( (millis() - started) > ETHERNET_TIMEOUT ) {
      Serial.println("\nCannot connect to Ethernet!");
      connected = false;
      startWiFi();
      return;
    }
  }
  createApi();
  server.begin();
}

void startWiFi() {
  WiFi.disconnect(true);
  if ( ssid != "" && password != "" ) {
    Serial.println("Accessing WiFi: " + String(ssid));
    WiFi.begin(ssid.c_str(), password.c_str());
    int started = millis();
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      if ( (millis() - started) > WIFI_TIMEOUT ) {
        Serial.println("Cannot connect to WiFi");
        connected = false;
        return;
      }
    }
    createApi();
    server.begin();
    connected = true;
    connectionType = "WiFi";
    Serial.print("Connected to WiFi at ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("No WiFi credentials");
    connected = false;
  }
}

void stopWiFi() {
  WiFi.disconnect(true);
  connected = false;
  connectionType = "";
}

//////////
// HTTP //
//////////

void sendHttp(String method, String url) {
  sendHttp(method, url, "HELLO");
}

void sendHttp(String method, String url, String payload) {
  if (connected) {
    if ( verbose ) {
      Serial.print("HTTP ===> ");
      Serial.print(method);
      Serial.print(" ");
      Serial.print(url);
      Serial.print(" { ");
      Serial.print(payload);
      Serial.print(" } ");
    }

    HTTPClient http;
    http.begin(url);

    int httpResponseCode;
    if ( method == "post" ) {
      httpResponseCode = http.POST(payload);
    } else if ( method == "put" ) {
      httpResponseCode = http.PUT(payload);
    } else {
      httpResponseCode = http.GET();
    }

    if (httpResponseCode > 0) {
      if ( verbose ) {
        Serial.print("OK: ");
        Serial.println(httpResponseCode);
      }
    }
    else {
      if ( verbose ) {
        Serial.print("Error: ");
        Serial.println(httpResponseCode);
      }
    }
    http.end();
  }
  else {
    Serial.println("Cannot Send HTTP - Network Disconnected");
  }
}


/////////
// OSC //
/////////

void loopOsc() {
  if ( connected ) OscWiFi.update();
}

void sendOsc(String host, int port, String address, int argument) {
  if ( connected ) {
    if ( verbose ) {
      Serial.print("OSC ===> ");
      Serial.print(host);
      Serial.print(":");
      Serial.print(port);
      Serial.print(address);
      Serial.print(" { ");
      Serial.print(argument);
      Serial.println(" } ");
    }
    OscWiFi.send(host, port, address, argument);
  } else {
    if ( verbose ) {
      Serial.println("Disconnected! Cannot send OSC");
    }
  }
}


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
