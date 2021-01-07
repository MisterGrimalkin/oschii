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
  void print() {
    Serial.println("   + Input:GPIO");
    Serial.print  ("     [pin:");
    if ( i2cPort >= 0 ) {
      Serial.print(i2cPort);
      Serial.print("/");
    }
    Serial.print(pin);
    Serial.print(" resistor:");
    Serial.print(resistor);
    Serial.print(" on:");
    Serial.print(onValue);
    Serial.print(" off:");
    if ( offValue == -1 ) {
      Serial.print("(none)");
    } else {
      Serial.print(offValue);
    }
    Serial.print(" invert:");
    Serial.print(invert);
    Serial.print(" bounce:");
    Serial.print(bounceFilter);
    Serial.println("]");
  }
};
GpioSensor gpioSensors[INPUTS_LIMIT];

struct AnalogSensor {
  int pin;
  void print() {
    Serial.println("   + Input:Analog");
    Serial.print  ("     [pin:");
    Serial.print(pin);
    Serial.println("]");
  }
};
AnalogSensor analogSensors[INPUTS_LIMIT];

struct UltrasonicSensor {
  int triggerPin;
  int echoPin;
  int samples;
  bool invert;
  void print() {
    Serial.println("   + Input:Ultrasonic");
    Serial.print  ("     [trig:");
    Serial.print(triggerPin);
    Serial.print(" echo:");
    Serial.print(echoPin);
    Serial.print(" samples:");
    Serial.print(samples);
    Serial.print(" invert:");
    Serial.print(invert);
    Serial.println("]");
  }
};
UltrasonicSensor ultrasonicSensors[INPUTS_LIMIT];

struct InfraredSensor {
  int pin;
  int samples;
  int minValue;
  void print() {
    Serial.println("   + Input:Infrared");
    Serial.print  ("     [pin:");
    Serial.print(pin);
    Serial.print(" samples:");
    Serial.print(samples);
    Serial.println("]");
  }
};
InfraredSensor infraredSensors[INPUTS_LIMIT];

struct SonarSensor {
  int pin;
  int samples;
  void print() {
    Serial.println("   + Input:Sonar");
    Serial.print  ("     [pin:");
    Serial.print(pin);
    Serial.print(" samples:");
    Serial.print(samples);
    Serial.println("]");
  }
};
SonarSensor sonarSensors[INPUTS_LIMIT];

//struct SonarRingSensor {
//  int pins[];
//  int directions[];
//  int distances[];
//
//  SonarRingSensor(JsonObject inputJson) {
//  }
//};

struct Receiver {
  String ip;
  int oscPort;
  String oscAddress;
  String httpMethod;
  String httpPath;
  void print() {
    Serial.print("     + Receiver:");
    if ( oscPort > 0 ) {
      Serial.println("OSC");
      Serial.print("       [ip:");
      Serial.print(ip);
      Serial.print(" port:");
      Serial.print(oscPort);
      Serial.print(" address:");
      Serial.print(oscAddress);
      Serial.println("]");
    } else {
      Serial.println("HTTP");
      Serial.print("       [ip:");
      Serial.print(ip);
      Serial.print(" method:");
      Serial.print(httpMethod);
      Serial.print(" path:");
      Serial.print(httpPath);
      Serial.println("]");
    }
  }
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
  void print() {
    Serial.println("     + Controller:GPIO");
    Serial.print  ("       [pin:");
    if ( i2cPort >= 0 ) {
      Serial.print(i2cPort);
      Serial.print("/");
    }
    Serial.print(pin);
    Serial.print(" on:");
    Serial.print(onState);
    Serial.print(" threshold:");
    Serial.print(valueThreshold);
    Serial.print(" invert:");
    Serial.print(invert);
    Serial.print(" follow:");
    Serial.print(follow);
    Serial.print(" toggle:");
    Serial.print(toggle);
    Serial.print(" pulse:");
    Serial.print(pulseLength);
    Serial.println("]");
  }
};
GpioController gpioControllers[CONTROLLERS_LIMIT];

struct PwmController {
  int i2cPort;
  int pin;
  int channel;
  String valueTransform;
  bool invert;
  void print() {
    Serial.println("     + Controller:PWM");
    Serial.print  ("       [pin:");
    if ( i2cPort >= 0 ) {
      Serial.print(i2cPort);
      Serial.print("/");
    }
    Serial.print(pin);
    Serial.print(" transform:");
    Serial.print(valueTransform);
    Serial.print(" invert:");
    Serial.print(invert);
    Serial.println("]");
  }
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

const int MAX_PATTERN_SIZE = 30;

struct Pattern {
  int size;
  bool repeat;
  bool fade;
  bool cosine;
  int startedAt;
};
Pattern patterns[INPUTS_LIMIT];

int patternValues[INPUTS_LIMIT][MAX_PATTERN_SIZE];
int patternTimes[INPUTS_LIMIT][MAX_PATTERN_SIZE];

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
    scanPatterns();
    scanSensors();

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

  if ( verbose ) gpioSensor.print();

  return value;
}

int buildUltrasonicSensor(int index, JsonObject inputJson) {
  errorMessage = "";

  int triggerPin = -1;
  int echoPin = -1;
  int samples = 1;
  bool invert = false;

  if ( inputJson.containsKey("triggerPin") )  triggerPin = inputJson["triggerPin"];
  if ( inputJson.containsKey("echoPin") )     echoPin =    inputJson["echoPin"];
  if ( inputJson.containsKey("samples") )     samples =    inputJson["samples"];
  if ( inputJson.containsKey("invert") )      invert =     inputJson["invert"];

  if ( triggerPin < 0 ) {
    errorMessage = "ERROR! Input " + String(index) + ": No trigger pin specified";
    return -1;
  }
  if ( echoPin < 0 ) {
    errorMessage = "ERROR! Input " + String(index) + ": No echo pin specified";
    return -1;
  }

  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  UltrasonicSensor ultrasonicSensor = {
    triggerPin, echoPin, samples, invert
  };
  ultrasonicSensors[index] = ultrasonicSensor;

  if ( verbose ) ultrasonicSensor.print();

  return invert ? 100 : 0;
}

int buildAnalogSensor(int index, JsonObject inputJson) {
  errorMessage = "";

  int pin = -1;

  if ( inputJson.containsKey("pin") ) pin = inputJson["pin"];

  if ( pin < 0 ) {
    errorMessage = "ERROR! Input " + String(index) + ": No pin specified";
    return -1;
  }

  pinMode(pin, INPUT);

  AnalogSensor analogSensor = {
    pin
  };
  analogSensors[index] = analogSensor;

  if ( verbose ) analogSensor.print();

  return 0;
}

int buildSonarSensor(int index, JsonObject inputJson) {
  errorMessage = "";

  int pin = -1;
  int samples = 1;

  if ( inputJson.containsKey("pin") ) pin = inputJson["pin"];
  if ( inputJson.containsKey("samples") ) samples = inputJson["samples"];

  if ( pin < 0 ) {
    errorMessage = "ERROR! Input " + String(index) + ": No pin specified";
    return -1;
  }

  pinMode(pin, INPUT);

  SonarSensor sonarSensor = {
    pin, samples
  };
  sonarSensors[index] = sonarSensor;

  if ( verbose ) sonarSensor.print();

  return 0;
}

int buildInfraredSensor(int index, JsonObject inputJson) {
  errorMessage = "";

  int pin = -1;
  int samples = 3;
  int minValue = 0;

  if ( inputJson.containsKey("pin") ) pin = inputJson["pin"];
  if ( inputJson.containsKey("samples") ) samples = inputJson["samples"];
  if ( inputJson.containsKey("minValue") ) minValue = inputJson["minValue"];

  if ( pin < 0 ) {
    errorMessage = "ERROR! Input " + String(index) + ": No pin specified";
    return -1;
  }

  InfraredSensor infraredSensor = {
    pin, samples, minValue
  };
  infraredSensors[index] = infraredSensor;

  if ( verbose ) infraredSensor.print();

  return 0;
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

  if ( verbose ) gpioController.print();

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

  if ( verbose ) pwmController.print();

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

  if ( verbose ) receiver.print();

  return true;
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

void scanPatterns() {
  for ( int i=0; i<INPUTS_LIMIT; i++ ) {
     Pattern * pattern = &patterns[i];
     if ( pattern->startedAt > 0 ) {
        int now = millis() - pattern->startedAt;
        int acc = 0;
        int patternComplete = true;
        for ( int t=0; t < pattern->size; t++ ) {
          int previousTime = acc;
          acc += patternTimes[i][t];
          if ( now < acc ) {
            int value = patternValues[i][t];
            if ( pattern->fade ) {
              int previousValue = 0;
              if ( t > 0 ) previousValue = patternValues[i][t-1];

              double progress = (double)(now - previousTime) / patternTimes[i][t];

              if ( pattern->cosine ) {
                value = cosineInterpolate(previousValue, value, progress);
              } else {
                value = linearInterpolate(previousValue, value, progress);
              }
            }
            Sensor * sensor = &sensors[i];
            sensor->changed = true;
            sensor->value = value;
            patternComplete = false;
            break;
          }
        }
        if ( patternComplete && pattern->repeat ) {
          pattern->startedAt = millis();
        }
     }
  }
}

double linearInterpolate(double v1, double v2, double amount) {
  return v1 + (amount * (v2 - v1));
}

double cosineInterpolate(double v1, double v2, double amount) {
  double mu = (1-cos(amount*PI))/2;
  return(v1*(1-mu)+v2*mu);
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

  } else if ( sensor->type == "analog" ) {
    AnalogSensor * analogSensor = &analogSensors[sensorIndex];

    int reading = analogRead(analogSensor->pin);
    int value = 100 * (reading / 4095.0);
    if ( value != sensor->value ) {
      sensor->value = value;
      sensor->changed = true;
      sensor->lastChangedAt = millis();
    }

  } else if ( sensor->type == "sonar" ) {
    SonarSensor * sonarSensor = &sonarSensors[sensorIndex];

    int value = readSonar(sonarSensor->pin, sonarSensor->samples);
    if ( value != sensor->value ) {
      sensor->value = value;
      sensor->changed = true;
      sensor->lastChangedAt = millis();
    }

  } else if ( sensor->type == "ultrasonic" ) {
    UltrasonicSensor * ultrasonicSensor = &ultrasonicSensors[sensorIndex];

    int reading = sampleUltrasonic(
                          ultrasonicSensor->triggerPin,
                          ultrasonicSensor->echoPin,
                          ultrasonicSensor->samples
                  );
    if ( ultrasonicSensor->invert ) reading = 100 - reading;
    if ( reading != sensor->value ) {
      sensor->value = reading;
      sensor->changed = true;
      sensor->lastChangedAt = millis();
    }

  } else if ( sensor->type == "infrared" ) {
    InfraredSensor * infraredSensor = &infraredSensors[sensorIndex];

    int reading = sampleInfrared(
                      infraredSensor->pin,
                      infraredSensor->samples
                  );
    if ( reading < infraredSensor->minValue ) {
      reading = 0;
    }
    if ( reading != sensor->value ) {
      sensor->value = reading;
      sensor->changed = true;
      sensor->lastChangedAt = millis();
    }

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

        } else if ( sensorType == "analog" ) {
          value = buildAnalogSensor(sensorCount, inputJson);
          if ( value < 0 ) {
            return errorMessage;
          }

        } else if ( sensorType == "sonar" ) {
          value = buildSonarSensor(sensorCount, inputJson);
          if ( value < 0 ) {
            return errorMessage;
          }

        } else if ( sensorType == "ultrasonic" ) {
          value = buildUltrasonicSensor(sensorCount, inputJson);
          if ( value < 0 ) {
            return errorMessage;
          }

        } else if ( sensorType == "infrared" ) {
          value = buildInfraredSensor(sensorCount, inputJson);
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
      int size = 0;
      bool repeat = false;
      bool fade = false;
      bool cosine = false;
      int startedAt = -1;

      if ( m.size() <= 1 ) {
        int value = m.arg<int>(0);
        if ( verbose ) {
          Serial.print(value);
          Serial.println(" <--- OSC");
        }
        Sensor * sensor = &sensors[sensorNo];
        sensor->value = value;
        sensor->changed = true;

      } else {
        if ( verbose ) {
          Serial.println("(...) <--- OSC");
        }
        String type = m.arg<String>(0);
        String command = m.arg<String>(1);
        cosine = type=="cosine";
        fade = type=="linear" || cosine;
        repeat = command=="repeat";
        for ( int i = 2; i < (m.size()-1); i+=2 ) {
          int value = m.arg<int>(i);
          int time = m.arg<int>(i+1);
          patternValues[sensorNo][size] = value;
          patternTimes[sensorNo][size] = time;
          size++;
        }
        startedAt = millis();
      }
      Pattern pattern = {
        size, repeat, fade, cosine, startedAt
      };
      patterns[sensorNo] = pattern;
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


/////////////
// HC-SR04 //
/////////////

const int HC_SR04_MAX = 2500;
const int HC_SR04_MIN = 200;

int sampleUltrasonic(int trig, int echo, int samples) {
  int readings[samples];
  int lastReading = 0;
  for ( int i = 0; i<samples; i++) {
    int reading = readUltrasonic(trig, echo);
    readings[i] = reading >= HC_SR04_MAX ? lastReading : reading;
    lastReading = reading;
  }
  qsort(readings, samples, sizeof(int), intCompare);
  int median = readings[samples/2];
  if ( median >= HC_SR04_MAX || median <= HC_SR04_MIN ) {
    return 0;
  } else {
    int adjusted = ((HC_SR04_MAX - median + HC_SR04_MIN) * 100.0) / (HC_SR04_MAX - HC_SR04_MIN);
    if ( adjusted >= 100 ) {
      return 100;
    } else {
      return adjusted;
    }
  }
}

int readUltrasonic(int trig, int echo) {
  digitalWrite(echo, LOW);
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  return pulseIn(echo, HIGH);
}

int intCompare (const void * a, const void * b) {
  return ( *(int*)a - *(int*)b );
}


//////////////////////
// Sharp GP2Y0A710F //
//////////////////////

int lastValue = 0;


// Short range: Sharp GP2Y0A02YK0F  20 to 150 cm
// Long range:  Sharp GP2Y0A710K0F 100 to 550

const int IR_SHORT = 0;    // Sharp GP2Y0A02YK0F   20 to 150 cm
const int IR_LONG = 1;     // Sharp GP2Y0A710K0F  100 to 550 cm

int irType = IR_SHORT;

double readVoltage(int pin) {
  int reading = analogRead(pin);
  return (reading / 4095.0) * 3.3;
}

int sampleInfrared(int pin, int samples) {
  int readings[samples];
  int lastReading = 0;
  for ( int i = 0; i<samples; i++) {
    int reading = readInfrared(pin);
    readings[i] = reading;
    lastReading = reading;
  }
  qsort(readings, samples, sizeof(int), intCompare);
  return readings[samples/2];
}

int readInfrared(int pin) {
  double milliVolts = readVoltage(pin) * 1000;
  if ( irType == IR_LONG ) {
    return 1 / ((milliVolts - 1125) / 137500);
  } else if ( irType == IR_SHORT ) {
    int distance = shortRangeIR(milliVolts);
    return 100 - ((float)(distance - 15) / (150 - 15)) * 100;
  }
}

const int TABLE_ENTRIES = 12;
const int INTERVAL  = 250;
static int distance[TABLE_ENTRIES] = {150,140,130,100,60,50,40,35,30,25,20,15};

int shortRangeIR(int mV) {
   if (mV > INTERVAL * TABLE_ENTRIES - 1) {
      return distance[TABLE_ENTRIES - 1];
   } else {
      int index = mV / INTERVAL;
      float frac = (mV % 250) / (float)INTERVAL;
      return distance[index] - ((distance[index] - distance[index + 1]) * frac);
   }
}


///////////
// SONAR //
///////////


double readMilliVolts(int pin) {
  int reading = analogRead(pin);
  return (reading / 4095.0) * 3300;
}

double sampleMilliVolts(int pin, int samples) {
  double readings[samples];
  double lastReading = 0;
  for ( int i = 0; i<samples; i++) {
    double reading = readMilliVolts(pin);
    readings[i] = reading;
    lastReading = reading;
  }
  qsort(readings, samples, sizeof(int), intCompare);
  return readings[samples/2];
}


const double MV_PER_MM = 9.8 / 25.4;

int readSonar(int pin) {
  return readSonar(pin, 1);
}

int readSonar(int pin, int samples) {
  double milliVolts;
  if ( samples > 1 ) {
    milliVolts = sampleMilliVolts(pin, samples);
  } else {
    milliVolts = readMilliVolts(pin);
  }
  int distance = milliVolts * MV_PER_MM;
  return distance;
}


