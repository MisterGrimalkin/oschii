# Oschii

Oschii for ESP32 allows communication with customised
hardware using common network protocols.

Connect sensors and controllers to the ESP32 and integrate
them with your media installation.

#### Sensors 
- **Digital GPIO**
  - ESP32 or MCP23017 I<sup>2</sup>C GPIO Expander
- **Ultrasonic Range Sensor**
  - Adafruit HC-SR04
- **Infrared Range Sensor**
  - Sharp GP2Y0A710F

#### Controllers
- **Digital GPIO**
  - ESP32 or MCP23017 I<sup>2</sup>C GPIO Expander
- **PWM**
  - ESP32 or PCA9685 I<sup>2</sup>C PWM Generator

#### Network protocols
- **O**pen **S**ound **C**ontrol
- **H**yper**T**ext **T**ransfer **P**rotocol


## Serial API

Oschii can accept commands over a serial connection, using PuTTy or Arduino IDE or similar,
and will respond with strings written to serial output. Some commands require subsequent inputs.

Set the line ending mode of your serial program to 'No line ending'.

#### Simple health check

- Send:     `poke`
- Response: `Tickles!`

#### Oschii Name

Oschii stores a name in internal storage, for easy identification.

**Get current name:**
- Send:     `name`
- Response: `[NAME OF OSCHII]`

**Set name:**
- Send:     `set name`
- Response: `Ready for name`
- Send:     `[NAME OF OSCHII]`
- Response: `Name is now [NAME OF OSCHII]`

#### Networking

Oschii can connect to WiFi or wired Ethernet (if available), but not both simultaneously.

**Get IP of Oschii:**
- Send:     `ip`
- Response: `[IP ADDRESS OF OSCHII] (WiFi | Ethernet)` | `DISCONNECTED`

**Get WiFi network name:**
- Send:     `ssid`
- Response: `[WIFI NETWORK NAME]`

_Oschii will return the SSID saved to internal storage, regardless of whether or not it is connected to WiFi_

#### To use WiFi

- Send:     `start wifi`
- Response: `Ready for ssid`
- Send:     `[WIFI NETWORK NAME]`
- Response: `Ready for password`
- Send:     `[WIFI PASSWORD]`
- Response: `Accessing WiFi: [WIFI NETWORK NAME]`

**If connection is successful:**

- Response: `Connected to WiFi at [IP ADDRESS OF OSCHII]`
 
**If connection fails:**

- Response: `Cannot connect to WiFi`

#### To use Ethernet

- Send: `start ethernet`
- Response: `Accessing Ethernet` plus probably loads of dots

**If connection is successful:**

- Response: _Some shit about MAC addresses and suchlike_ 

Oschii will now try to connect to Ethernet on every startup.

**If connection fails:**

- Response: `Cannot connect to Ethernet!`

Oschii will then try to connect to WiFi, but will try Ethernet again on startup.

#### To disable Ethernet

- Send: `stop ethernet`
- Response: `Restart ESP now`

Do so. Oschii will now NOT try to connect to Ethernet on startup.

#### Configuration

**Get current configuration:**
- Send: `config`
- Response: JSON configuration document, as saved to internal storage

**Set new configuration:**
- Send: `set config`
- Response: `Ready for config`
- Send: JSON document

See the section below for the required JSON format.

## HTTP API

Oschii offers HTTP endpoints to get and set the device's name and
configuration.

**Get Name:**

**`GET`** `http://OSCHII_IP/name`

e.g. `curl http://192.168.1.123/name`

**Set Name:**

**`POST`** `http://OSCHII_IP/name` with name as payload

e.g. `curl -X POST -d "Bob" http://192.168.1.123/name`

**Get Config:**

**`GET`** `http://OSCHII_IP/config`

e.g. `curl http://192.168.1.123/config`

**Set Config:**

**`POST`** `http://OSCHII_IP/config` with config JSON as payload

e.g. `curl -X POST -d "@my_config.json" http://192.168.1.123/config`

When config is received Oschii will attempt to parse and validate
the incoming JSON document.
- If there are errors, Oschii will respond with HTTP status `400` 
and an error message. 
- If config is valid, Oschii will respond with HTTP status `200` 
and the message "Got it. Sounds fun!". It will then save the JSON
to internal storage and reboot.

## OSC API

Oschii provides an OSC listener on port 3333, address: `/hello_oschii`,
for integration with OschiiCloud.

When an OSC message is received on this address, Oschii will save the
remote IP address (referred to as 'Cloud IP') and send a message back, 
containing its name, to port 3333 and address: `/i_am_oschii`.

If Cloud IP is set, Oschii will perform this OSC operation 
on every startup.

You can refer to the Cloud IP in Receivers similarly to Devices:

`"device": "_CLOUD_"`


---

## Configuration

Oschii stores its configuration in a JSON file. You can read and overwrite
this file via a serial connection or with a HTTP request.

### Configuration via HTTP

Get config with:

**`GET`** `http://OSCHII_IP/config`

e.g. `curl http://192.168.1.123/config`

Set config with:

**`POST`** `http://OSCHII_IP/config` with config JSON as payload

e.g. `curl -X POST -d "@my_config.json" http://192.168.1.123/config`

## JSON

The Oschii configuration JSON document has this structure:

```
{
    "devices": [ ... ],     
    "inputs": [ ... ],
    "outputs": [ ... ]
}
```

- `devices`: Devices on the network that will receive messages from Oschii
- `inputs`: Mappings from Oschii's physical inputs to network messages 
- `outputs`: Mappings from network messages to Oschii's physical outputs

## Devices

The `devices` array defines machines on the network to which Oschii will send messages when an input's value changes. 
Each Device has a name and an IP address.

```
{
    "name": "Laptop",
    "ip": "192.168.1.101"
}
```

You can refer to a Device by name (e.g. `"device": "Laptop"`) elsewhere in the configuration
and its IP address will be read from this list.

You can also use the pre-defined Device symbol `_CLOUD_` to refer to the
OschiiCloud server (if set).

 
## Inputs

The `inputs` array sets up Oschii's physical inputs. As well as defining the physical characteristics of the sensor
it specifies an array of `receivers` and `controllers`. When the value being read
by the Input sensor changes, the new value will be sent to all the Receivers and
Controllers associated with the input.

##### Digital GPIO

Digital GPIO reads either HIGH or LOW voltage on an input pin.

```
{
    "type": "gpio",         
    "i2cPort": 0,           
    "pin": 0,               
    "resistor": "off",      # default="off" | "up" | "down"
    "onValue": 1,           # default=1
    "offValue": 0,          # default=(none)
    "invert": false,        # default=false
    "bounceFilter": 100,    # default=100
    "receivers": [ ... ],
    "controllers": [ ... ]
}
```

Specify `i2cPort` to use the I<sup>2</sup>C Expander (Port A: `0`, Port B: `1`). Otherwise native GPIOs will be used. 

Specify `resistor` to enable the pull-up (`"up"`) or pull-down (`"down"`) resistor on the input pin, or bypass both
(`"off"`). Pull-down resistors are **not available** on the I<sup>2</sup>C expander.

The Sensor will send `onValue` when the input goes HIGH, and `offValue` when it goes LOW. By default there is no
`offValue` and the Sensor will not send anything when the input goes LOW.

Setting `invert` to `true` will swap the behaviour of HIGH and LOW.

Once the Sensor has sent its value, it will ignore subsequent pin changes for `bounceFilter` milliseconds.

NOTE: When resistor mode is `"up"`, the behaviour of HIGH and LOW will be automatically inverted. You can re-invert
using the `invert` flag.


##### Analog

Analog input reads the input voltage on a native GPIO pin, in the range
0 to 3.3 Volts.

```
{
    "type": "analog",
    "pin": 0
}
```


##### Ultrasonic Range Sensor (Adafruit HC-SR04)

Measures a distance value from ultrasonic sensor. This requires a Trigger pin
and an Echo pin, which must be native GPIO.

```
{
    "type": "ultrasonic",         
    "triggerPin": 5,           
    "echoPin": 6,
    "samples": 1,             # default=1
    "invert": false,          # default=false
    "receivers": [ ... ],
    "controllers": [ ... ]
}
```

The **HC-SR04** sensor will read a distance in range 0..100,
proportional to the closeness of the object.

The sensor value is the median of `samples` readings.
 
Set `invert` to `true` to invert the range, ie. min distances
returns `0` instead of `100`.

##### Infrared Range Sensor (Sharp GP2Y0A710F)

Measure a distance value from infrared sensor.

```
{
    "type": "range-ir",
    "pin": 5
}
```

Reads distance in range 0..100

---

### `"receivers": [ ... ]`

The `receivers` array specifies the Devices that will receive a message when the Sensor value changes. 

**To receive as OSC** (recommended):

```
{
    "device": "Laptop",             # Defined in "devices" array
    "oscPort": 3333,                # default=3333 
    "oscAddress": "/oschii/in0"    
}
```

The Device should listen on `oscPort` and `oscAddress` for incoming messages.
The new Sensor value will be the first argument in the OSC message.

**To receive as HTTP**:

```
{
    "device": "Laptop",             # Defined in "devices" array
    "httpMethod": "post",           # default="post" | "put" | "get"
    "httpPath": "/oschii/in0"
}
```

The Device should provide an endpoint at `httpPath` for `httpMethod` requests. 
For POST and PUT requests the new Sensor value will be the request payload. For GET requests there will be no value.

### `"controllers": [ ... ]`

The `controllers` array specifies physical outputs that will be updated with the new Sensor value.

##### Digital GPIO

Digital GPIO output pins can be set to HIGH or LOW voltage.

```
{
    "type": "gpio"              
    "i2cPort": 0,         
    "pin": 0,
    "onState": true,            # default=true
    "valueThreshold": 1,        # default=1
    "invert": false,            # default=false
    "follow": true,             # default=true
    "toggle": false,            # default=false
    "pulseLength": 0,           # default=0
    "initialState": false       # default=(none) 
}
```

Specify `i2cPort` to use the I<sup>2</sup>C Expander (Port A: `0`, Port B: `1`). Otherwise native GPIOs will be used. 

Output will be set to `onState` when the incoming value is `>=` `valueThreshold`.

If `invert` is `true` then the output will be set to `onState` when the value is **`<`** `valueThreshold`.

If `follow` is `true` then an incoming value `<` `valueThreshold` will set the output to `NOT onState`. This
behaviour can also be inverted.

If `toggle` is `true` then `onState` will invert itself after every change.

If `pulseLength` is specified, the output will be set to `onState` for `pulseLength` milliseconds and then return
to the opposite state. Pulse is disabled if `toggle` is `true` because that would get silly.

If `initialState` is specified, Oschii will initialize the output pin to that state when config loads.

##### Pulse Width Modulation

PWM outputs send a modulated square wave from a pin.

```
{
    "type": "pwm"              
    "i2cPort": 0,               
    "pin": 0,
    "valueTransform": "none",   # default="none" | "percentage" | "binary" | "8bit"
    "initialValue": 0,          # default=(none) 
    "invert": false             # default=false
}
```

Specify `i2cPort` to >= 0 to use the I<sup>2</sup>C PWM expander (the value itself is ignored). Otherwise native
GPIOs will be used.

The `valueTransform` property determines how the Controller interprets the value it receives:
- `none`: Pass the Sensor's integer value directly to the output pin (0..255 for native, 0..4095 for I<sup>2</sup>C)
- `percentage`: Treat the incoming value as a percentage (0..100) of the maximum output value
- `binary`: Treat any incoming value `>= 1` as the maximum output value
- `8bit`: Treat the incoming value as an 8-bit integer (0..255) proportion of the maximum output value


If `initialValue` is set, the Output will be initialised to that value.

The `invert` flag only applies to I<sup>2</sup>C PWMs.

## Outputs

In this array you define Controllers you can activate with network messages (OSC
or HTTP). You can also specify Receivers to which Oschii will forward the trigger messages.

##### Using OSC

```
{
    "oscAddress": "/oschii/out0",
    "oscPort": 3333,                # default=3333
    "controllers": [ ... ],
    "receivers": [ ... ]
}
```

##### Using HTTP

```
{
    "httpPath": "/out0",
    "httpMethod": "post",           # default="post" | "get" | "put" | "delete"
    "controllers": [ ... ],
    "receivers": [ ... ]
}
```

The formats for objects in the `controllers` and `receivers` arrays are the same as described in the `inputs` section. 
