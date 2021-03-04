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
  
  ...
  
  "initialValue": -1
}
```

Driver output value will be set to `initialState` on startup
if it is `>= 0`.

### Driver Type `gpio`

_Sets HIGH or LOW voltage on a GPIO pin._

##### Minimal Configuration:
```
{
  "name": "Lamp 1",
  "type": "gpio",
  "pin": 16
}
```

**Default behaviour**: Output voltage will be set HIGH when the incoming
value is `> 0`, and LOW when the value is `== 0`.

##### Full configuration (showing defaults for non-required fields):
```
{
  "name": "Lamp 1",   # required
  "type": "gpio",     # required
  "initialValue": -1,
  "pin": 16,          # required
  
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

##### Minimal Configuration:
```
{
  "name": "Lamp 2",
  "type": "pwm",
  "pin": 19
}
```

Yeah that's pretty much it.


