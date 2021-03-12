# Drivers

_A Driver writes an integer value to some physical device._

Every Driver needs a `name` and `type`.

The `name` must be unique, and the `type` must be one of:

- `gpio`
- `pwm`

##### General configuration:
```
{
  "name": "....",
  "type": "....",
  "i2cModule": "....",    # only for I2C
  
  ...
  
  "initialValue": 0,
  "invert": false
}
```

Driver output value will be set to `initialState` on startup
if it is `>= 0`.

### Driver Type `gpio`

_Sets HIGH or LOW voltage on a GPIO pin._

```
{
  "name": "Lamp 1",       # required
  "type": "gpio",         # required
  "i2cModule": "....",    # only for I2C

  "pin": 16,              # required
  
  "initialValue": 0,
  
  "thresholdValue": 1,
  "thresholdHighPass": true,
  "invert": false
}
```

Use `thresholdValue` and `thresholdHighPass` to specify the values
which will trigger a HIGH output voltage.

Set `invert` to `true` to swap HIGH and LOW voltages.

### Driver Type `pwm`

_Outputs PWM wave on a GPIO pin._

```
{
  "name": "Lamp 2",       # required
  "type": "pwm",          # required
  "i2cModule": "....",    # only for I2C

  "valueTransform": {},

  "pin": 19,              # required
  
  "initialValue": 0,
  "invert": false         # only for I2C
}
```
