# Remotes

_A Remote takes an integer value (from a Sensor Monitor or an incoming network message)
and uses it to control a Driver. The value may be passed directly or interpreted by
a Cadence._

##### Configuration:
```
"driverRemotes": {
  "/remote1": {
    "type": "osc",
    "writeTo": [
      {
        "driver": "Lamp 1",
        "valueMultiplier": 1.0,
        "valueOffset": 0
      }
    ],
    "cadence": [
    ]
  }
}
```