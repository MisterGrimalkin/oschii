# Sensors

_A Sensor reads some physical input and returns an integer value. 
If the value has changed since the last reading, the Sensor will send
 a "Changed" message to any Sensor Monitors attached to it._
   
Every Sensor needs a `name` and `type`.

The `name` must be unique, and the `type` must be one of:

- Binary Sensors:
  - `gpio`
  - `touch`
- Range Sensors:
  - `analog`
  - `hc-sr04`


## Binary Sensors

_A Binary Sensor reads an ON / OFF input and returns one of two integer values._

```
{
  "name": "....",
  "type": "...."
  "i2cModule": "....",

  ...
  
  "onValue": 100,
  "offValue": -1,
  "invert": false,
  "bounceFilter": 0,
  "holdOnFilter": 0
}
```

When the Sensor's input changes to ON it will send a "Changed" message with the value
`onValue`. Similarly for `offValue`. 

If the value is `-1` then the "Changed" message will **not** be sent.

Set `invert` to `true` to swap the behaviour of ON / OFF.

Once the value has changed, subsequent ON / OFF changes 
are ignored for `bounceFilter` milliseconds.

An ON reading will only be acted upon after being held for `holdOnFilter` milliseconds.

### Sensor Type `gpio`

_Measures HIGH or LOW voltage on a GPIO pin._

```
{
  "name": "Button 1",     # required
  "type": "gpio",         # required
  "i2cModule": "....",    # only for I2C

  "pin": 7,               # required
  
  "resistor": "off",      # "off" | "up" | "down"
  
  "onValue": 100,
  "offValue": -1,
  "invert": false,
  "bounceFilter": 0,
  "holdOnFilter": 0
}
```

The `resistor` setting optionally enables the PULL-UP or PULL-DOWN resistors
available on some GPIO pins.

### Sensor Type `touch`

_Measures a capacitive touch sensor._

```
{
  "name": "Pad 1",          # required
  "type": "touch",          # required
  "i2cModule": "....",      # NOT CURRENTLY SUPPORTED

  "pin": 2,                 # required
  
  "triggerThreshold": 15,
  "triggerHighPass": false,
  
  "onValue": 100,
  "offValue": -1,
  "invert": false,
  "bounceFilter": 0,
  "holdOnFilter": 0
}
```

This type of sensor actually returns an integer value in the range (0 - 100),
 which is itself meaningless but will change markedly when the sensor is touched. 
 The Sensor reads ON and OFF based on `triggerThreshold` and `triggerHighPass`. 
The default behaviour is to consider a value `<= 15` as ON.


## Range Sensors

_A Range Sensor returns an integer value within a specified range. 
The value can be from a single reading, or the median value from a number of samples._

##### General configuration:
```
{
  "name": "....",
  "type": "...."
  "i2cModule": "....",
  
  "valueTransform": {},

  ...
  
  "samples": 1, 
  "interleave": false,
}
```

If `interleave` is `false` then Oschii will take `samples` readings from the Sensor on each scan cycle and
return the median value. If `true`, Oschii will take one sample per scan cycle and only return the median
value once it has collected `samples` readings.

### Sensor Type `analog`

_Measures the analog voltage on an ADC pin._

```
{
  "name": "Rotary 1",       # required
  "type": "analog",         # required
  "i2cModule": "....",      # NOT CURRENTLY SUPPORTED

  "pin": 36,                # required
  
  "valueTransform": {},

  "samples": 1,
  "interleave": false,
}
```

### Sensor Type `hc-sr04`

_Measures the distance to an object with 
the **Adafruit HC-SR04** ultrasonic distance sensor._ 

Requires TRIG and ECHO to be connected.

```
{
  "name": "Ultrasonic 1",   # required
  "type": "hc-sr04",        # required
  "trigPin": 13,            # required
  "echoPin": 12,            # required
  
  "valueTransform": {},

  "samples": 9, 
  "interleave": true
}
```
