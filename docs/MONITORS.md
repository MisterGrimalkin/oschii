# Monitors

_A Monitor watches a Sensor. When the Sensor's value changes
(and/or at regular polling intervals) the Monitor will send
the value to the defined network Receivers, and to the
defined Remotes._

##### Configuration:
```
"sensorMonitors": [
  {
    "sensor": "Knob"
    "onChange": true,
    "pollInterval": 250,
    "sendTo": [
      {
        "MaxLaptop": "/oschii/knob",
        "type": "osc"
      },
      {
        "~": "/local/remote"
      },
      {
        "*": "/oschii/sensor0
      }
    ]
  }
]
```