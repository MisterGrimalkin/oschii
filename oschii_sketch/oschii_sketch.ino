#define VERSION "0.0.1"
#define BUILD_DATETIME __DATE__ " " __TIME__

#include <Adafruit_PWMServoDriver.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoOSC.h>

#include <Oschii.h>

#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))
#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))

bool reboot = false;

bool verbose = true;

String configPayload = "";

const int JSON_SIZE_LIMIT = 8192;

StaticJsonDocument<JSON_SIZE_LIMIT> doc;

const String CONFIG_OK = "Got it. Sounds fun!\n";

const String  DEFAULT_NAME = "Oschii";

const bool DIR_OUT = true;
const bool DIR_IN = false;

const bool PULL_UP = true;
const bool PULL_DOWN = false;

const int I2C_PWM_ADDR = 0x40;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(I2C_PWM_ADDR, Wire);
int pwmChannelCount = 0;

bool oschiiGO = false;

/////////////
// Structs //
/////////////

const int INPUTS_LIMIT = 64;
const int CONTROLLERS_LIMIT = 128;
const int RECEIVERS_LIMIT = 128;

struct Sensor {
  String type;
  bool simple;
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

struct TouchSensor {
  int pin;
  void print() {
    Serial.println("   + Input:Touch");
    Serial.print  ("     [pin:");
    Serial.print  (pin);
    Serial.println("]");
  }
};
TouchSensor touchSensors[INPUTS_LIMIT];

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

HCSRSensor hcsrSensors[INPUTS_LIMIT];

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
  int pin = -1;
  int samples = 1;
  String error = "";
  SonarSensor() {}
  SonarSensor(JsonObject inputJson) {
    if ( inputJson.containsKey("pin") ) pin = inputJson["pin"];
    if ( inputJson.containsKey("samples") ) samples = inputJson["samples"];
    if ( pin < 0 ) {
      error = "No pin specified";
    }
  }
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


FileService files;
SettingsService settings(files, "/settings.json");

static String configToLoad = "";

void setup() {
  settings.load();

  startSerial();
  startNetwork();
  startI2CGpio();
  startI2CPwm();

  const String config = files.readFile("/config.json");
//  const String config = "{}";

  if ( config != "" ) {
    parseJson(config);
  }

  pingCloud();

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

  gpioSensor.print();

  return value;
}

int buildTouchSensor(int index, JsonObject inputJson) {
  errorMessage = "";

  int pin = -1;

  if ( inputJson.containsKey("pin") ) pin = inputJson["pin"];

  if ( pin < 0 ) {
    errorMessage = "ERROR! Input " + String(index) + ": No pin specified";
    return -1;
  }

  TouchSensor touchSensor = {
    pin
  };
  touchSensors[index] = touchSensor;

  touchSensor.print();

  return 0;
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

  analogSensor.print();

  return 0;
}

int buildSonarSensor(int index, JsonObject inputJson) {
  errorMessage = "";

  SonarSensor sonarSensor = SonarSensor(inputJson);
  sonarSensors[index] = sonarSensor;

  if ( sonarSensor.error != "" ) {
    errorMessage = "ERROR! Input " + String(index) + ": " + sonarSensor.error;
    return -1;
  }

  pinMode(sonarSensor.pin, INPUT);

  sonarSensor.print();

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

  infraredSensor.print();

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

  gpioController.print();

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

  pwmController.print();

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

  receiver.print();

  return true;
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

void scanSensors() {
  pullI2CGpio();

  int i;
  for ( i=0; i<sensorCount; i++ ) {
    Sensor * sensor = readSensor(i);
    if ( sensor->changed ) {
      int value = sensor->value;
      if ( verbose ) {
        Serial.print("[#");
        Serial.print(i);
        Serial.print("] ---> ");
        Serial.println(value);
      }

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
          if ( sensor->simple ) {
            sendOsc(
              receiver->ip,
              receiver->oscPort,
              receiver->oscAddress,
              value
            );
          } else {
            String payload = "{\"distance\":"+String(value)+",\"direction\": 0,\"width\": 45}";
            sendOsc(
              receiver->ip,
              receiver->oscPort,
              receiver->oscAddress,
              payload
            );
          }
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

  } else if ( sensor->type == "touch" ) {
    TouchSensor * touchSensor = &touchSensors[sensorIndex];

    int reading = touchRead(touchSensor->pin);
    int value = reading < 50 ? 1 : 0;
    if ( value != sensor->value ) {
      sensor->value = value;
      sensor->changed = true;
      sensor->lastChangedAt = millis();
    }


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

  } else if ( sensor->type == "hc-sr04" ) {
    HCSRSensor * hcsr = &hcsrSensors[sensorIndex];
    hcsr->readSensor();
    if ( hcsr->hasChanged() ) {
      sensor->value = hcsr->getValue();
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
  Serial.println("\n== Oschii is receiving new configuration ==");
  oschiiGO = false;

  sensorCount = 0;

  const char * json = input.c_str();
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    String reply = "ERROR! Parsing JSON: " + String(error.c_str());
    Serial.println(reply);
    Serial.println("\n== Oschii could not comply :( ==\n");
    oschiiGO = true;

    return reply;
  }

  int i;

  if ( doc.containsKey("verbose") ) {
    verbose = doc["verbose"];
  }

  if ( doc.containsKey("devices") ) {
    Serial.println("\n-- Parsing Devices");
    String result = buildDevices(doc["devices"]);
    if ( result != "OK" ) {
      return result;
    }
    Serial.print("-- FOUND: ");
    Serial.println(getDeviceCount());
  } else {
    Serial.println("\n-- No Devices");
  }

  ///////// INPUTS //////////

  if (doc.containsKey("inputs")) {
    Serial.println("\n-- Parsing INPUTS");

    JsonArray inputsJson = doc["inputs"];
    for ( i = 0; i < INPUTS_LIMIT; i++ ) {
      JsonObject inputJson = inputsJson[i];

      if ( inputJson.containsKey("type") ) {

        /* Sensor */

        String sensorType = inputJson["type"];
        int value = -1;
        bool simple = true;

        if ( sensorType == "gpio" ) {
          value = buildGpioSensor(sensorCount, inputJson);
          if ( value < 0 ) {
            return errorMessage;
          }

        } else if ( sensorType == "touch" ) {
          value = buildTouchSensor(sensorCount, inputJson);
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
//          simple = false;
          if ( value < 0 ) {
            return errorMessage;
          }

        } else if ( sensorType == "hc-sr04" ) {
          HCSRSensor hcsr = HCSRSensor(sensorCount);
          if ( !hcsr.build(inputJson) ) {
            return hcsr.getError();
          }
          hcsrSensors[sensorCount] = hcsr;
          value = hcsr.getValue();
          hcsr.print();

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
          sensorType, simple, value, false, -1,
          inputControllerStartIndex, inputControllerCount,
          inputReceiverStartIndex, inputReceiverCount
        };
        sensors[sensorCount++] = sensor;
      }
    }
    Serial.print("-- FOUND: ");
    Serial.println(sensorCount);
  } else {
    Serial.println("\n-- No Inputs");
  }

  int inputCount = sensorCount;

  ///////// OUTPUTS //////////

  if ( doc.containsKey("outputs") ) {
    Serial.println("\n-- Parsing OUTPUTS");

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
        "output", true, -1, false, -1,
        outputControllerStartIndex, outputControllerCount,
        outputReceiverStartIndex, outputReceiverCount
      };
      sensors[sensorCount++] = sensor;
    }
    Serial.print("-- FOUND: ");
    Serial.println(sensorCount - inputCount);
  } else {
    Serial.println("\n-- No Outputs");
  }

  if (!files.writeFile("/config.json", input)) {
    Serial.println("\n== Oschii could not write config :( ==\n");
  }

  Serial.println("\n== Oschii is ready :D ==\n");
  oschiiGO = true;

  return CONFIG_OK;
}


/////////
// API //
/////////

void createApi() {
  createOscPing();
  createHttpApi();
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
  double value = (reading / 4095.0) * 3300;
  return value;
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
  double median = readings[samples/2];
  return median;
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
  int distance = (int)((milliVolts / 3300.0) * 100.0);
//  int distance = milliVolts * MV_PER_MM;
  return distance;
}
