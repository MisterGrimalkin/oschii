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

##### General configuration:
```
{
  "name": "....",
  "type": "...."

  "i2cModule": "....",    # optional

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

##### Minimal Configuration:
```
{
  "name": "Button 1",
  "type": "gpio",
  "pin": 7
}
```

**Default behaviour**: When the button (or similar) on GPIO `pin` is pressed, 
the Sensor will send a "Changed" message with the value `100`. 
When the button is released, the Sensor will do nothing.

##### Full Configuration (showing defaults for non-required fields):
```
{
  "name": "Button 1",     # required
  "type": "gpio",         # required
  "pin": 7,               # required
  "resistor": "off",      # "off" | "up" | "down"
  
  "i2cModule": "....",    # optional
  
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

##### Minimal Configuration:
```
{
  "name": "Pad 1",
  "type": "touch",
  "pin": 2
}
```

**Default behaviour**: When the conductive material connected to GPIO `pin` is touched by a human, the Sensor
will send a "Changed" message with the value `100`. When the touch is released, the
Sensor will do nothing.

##### Full Configuration (showing defaults for non-required fields):
```
{
  "name": "Pad 1",        # required
  "type": "touch",        # required
  "pin": 2,               # required
  
  "i2cModule": "....",    # NOT CURRENTLY SUPPORTED
  
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
  
  "i2cModule": "....",    # optional
  
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

##### Minimal configuration:
```
{
  "name": "Rotary 1",
  "type": "analog",
  "pin": 36
}
```

This will return the voltage on the specified native GPIO pin in the range (**0 - 3.3V**), 
scaled to a value in the range (**0 - 100**).

##### Full Configuration (showing defaults for non-required fields):
```
{
  "name": "Rotary 1",       # required
  "type": "analog",         # required
  "pin": 36,                # required
  
  "samples": 1,
  "interleave": false,
  
  "readingRange": [
    0,
    3300
  ],
  "discardOutliers": true,
  
  "valueRange": [
    0,
    100
  ],
  "flipRange": false,
  
  "bandPass": [
    0,
    100
  ],
  "bandCut": [
    -1,
    -1
  ],
}
```

### Sensor Type `hc-sr04`

_Measures the distance to an object with 
the **Adafruit HC-SR04** ultrasonic distance sensor._ 

The TRIG and ECHO pins on the device should be wired to native GPIO pins.

##### Minimal configuration:
```
{
  "name": "Ultrasonic 1",
  "type": "hc-sr04",
  "trigPin": 13,
  "echoPin": 12
}
```

This will return the distance to a (flat and smooth) object 
in the (very approximate) range (**3cm - 90cm**), 
scaled to a value in the range (**0 - 100**).
The closest distance is returned as 100, the farthest as 0.

##### Full Configuration (showing defaults for non-required fields):
```
{
  "name": "Ultrasonic 1",   # required
  "type": "hc-sr04",        # required
  "trigPin": 13,            # required
  "echoPin": 12,            # required
  
  "samples": 9, 
  "interleave": true
}
```

## Range conversion example

Imagine a sensor which returns a value in the range (0 - 10), but is only accurate in the range (2 - 6).
We take four readings, two of which are outside the stable range:

```
               0   1   2   3   4   5   6   7   8   9   10
 Stable Range          |||||||||||||||||
     READINGS      A           B   C                   D
```

We can cap the erratic readings by setting `readingRange` to `[2, 6]`:

```
               0   1   2   3   4   5   6   7   8   9   10
readingRange           [---------------]
                   *-->A       B   C   D<--------------* 
```

To return the value as a percentage of the stable range we set `valueRange` to `[0, 100]`:

```
              0   10   20   30   40   50   60   70   80   90   100
  valueRange  [--------------------------------------------------]
              A                       B            C             D
              
```

We can map the highest reading to the lowest value by setting `flipRange` to `true`:

```
              0   10   20   30   40   50   60   70   80   90   100
  valueRange  [--------------------------------------------------]
  (flipped)   D           C           B                          A
              
```

We can ignore readings that are outside `readingRange` (`A` and `D`) by
setting `discardOutliers` to `true`.