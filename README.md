# Oschii-Core

Oschii-Core for ESP32 allows communication with customised
hardware using common network protocols.

Connect sensors and controllers to the ESP32 and integrate
them with your media installation.

#### Sensors 
- **Digital GPIO**
  - ESP32 or MCP23017 I<sup>2</sup>C GPIO Expander

#### Controllers
- **Digital GPIO**
  - ESP32 or MCP23017 I<sup>2</sup>C GPIO Expander
- **PWM**
  - ESP32 or PCA9685 I<sup>2</sup>C PWM Generator

#### Network protocols
- **O**pen **S**ound **C**ontrol
- **H**yper**T**ext **T**ransfer **P**rotocol

## Configuration

Oschii stores its configuration in a JSON file. You can read and overwrite
this file via a serial connection or with a HTTP request.

### Configuration via Serial

Open a serial connection to Oschii.

1. Send string `config`
1. Oschii will respond with config JSON


1. Send string `set config`
1. Oschii will respond with `Ready for config`
1. Send JSON
1. Oschii will respond with OK or an error message

### Configuration via HTTP

Get config with:

**`GET`** `http://OSCHII_IP/config`

Set config with:

**`POST`** `http://OSCHII_IP/config` with config JSON as payload

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
 
 
## Inputs

The `inputs` array sets up Oschii's physical inputs. 
 
 
 _Sensors_ and an array of _Receivers_ and _Controllers_ for each.
When the value being read by the Sensor changes, the new value is passed to all the Receivers and Controllers.

Each `input` has this structure:

```
{
    "sensor": { ... },
    "receivers": [ ... ],
    "controllers": [ ... ]
}
```

### `"sensor": { ... }`

This object defines the physical characteristics of the Sensor input.

##### Digital GPIO

Digital GPIO reads either HIGH or LOW voltage on an input pin.

```
"sensor": {
    "type": "gpio",         
    "i2cPort": 0,           
    "pin": 0,               
    "resistor": "off",      # default="off" | "up" | "down"
    "onValue": 1,           # default=1
    "offValue": 0,          # default=(none)
    "invert": false,        # default=false
    "bounceFilter": 100     # default=100
}
```

Specify `i2cPort` to use the I<sup>2</sup>C Expander (Port A: `0`, Port B: `1`). Otherwise native GPIOs will be used. 

Specify `resistor` to enable the pull-up (`"up"`) or pull-down (`"down"`) resistor on the input pin, or bypass both
(`"off"`). Pull-down resistors are **not available** on the I<sup>2</sup>C expander.

The Sensor will send `onValue` when the input goes HIGH, and `offValue` when it goes LOW. By default there is no
`offValue` and the Sensor will not send anything when the input goes LOW.

Setting `invert` to `true` will swap the behaviour of HIGH and LOW.

Once the Sensor has sent its value, it will ignore subsequent pin changes for `bounceFilter` milliseconds. 

### `"receivers": [ ... ]`

The `receivers` array specifies the Devices that will receive a message when the Sensor value changes. 

**To receive as OSC** (recommended):

```
{
    "device": "Laptop",             # Defined in "devices" array
    "oscPort": 3333,                # default=3333 
    "oscAddress": "/oschii/in0",    
}
```

The Device should listen on `oscPort` and `oscAddress` for incoming messages.
The new Sensor value will be the first argument in the OSC message.

**To receive as HTTP**:

```
{
    "device": "Laptop",             # Defined in "devices" array
    "httpMethod": "post",           # default="post" | "put" | "get"
    "httpPath": "/oschii/in0",
}
```

The Device should provide an endpoint at `httpPath` for `httpMethod` requests. 
For POST and PUT requests the new Sensor value will be the request payload. For GET requests there will be no value.

### `"controllers": [ ... ]`



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

As with Sensors, defining the `i2cPort` value will tell Oschii to use the I<sup>2</sup>C Expander.

If `initialState` is specified, Oschii will initialize the output pin to that state when config loads.

Output will be set to `onState` when the incoming value is `>=` `valueThreshold`.

If `invert` is `true` then the output will be set to `onState` when the value is **`<`** `valueThreshold`.

If `follow` is `true` then an incoming value `<` `valueThreshold` will set the output to `NOT onState`. This
behaviour can also be inverted.

If `toggle` is `true` then `onState` will invert itself after every change.

If `pulseLength` is specified, the output will be set to `onState` for `pulseLength` milliseconds and then return
to the opposite state. Pulse is disabled if `toggle` is `true` because that would get silly.

##### Pulse Width Modulation

PWM outputs send a modulated square wave from a pin.

```
{
    "type": "pwm"              
    "i2cPort": 0,               
    "pin": 0,
    "valueTransform":           # default="none" | "percentage" | "binary"
    "initialValue": 0,          # default=(none) 
    "invert": false             # default=false
}
```

Most of the parameters are the same as for Digital GPIO outputs. Specifying `i2cPort` will tell Oschii
to use the I<sup>2</sup>C PWM Expander.

The `valueTransform` property determines how the Controller interprets the value it receives:
- `none`: Pass the Sensor's integer value directly to the output pin
- `percentage`: Treat the incoming value as a percentage (0-100) of the maximum output value
- `binary`: Treat any incoming value `>= 1` as the maximum output value

## Outputs

In this array you define Controllers you can then activate with network messages (OSC
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
