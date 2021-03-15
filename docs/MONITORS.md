# Monitors

_A Monitor watches a Sensor. When the Sensor's value changes
the Monitor will send
the value to defined network Receivers, and/or to
defined local Remotes._


```
"sensorMonitors": [
  {
    "sensor": "Knob"                  # required
    "onChange": true,                 # (polling mode tbc)
    
    "valueTransform": {},
    
    "sendTo": [
      {
        "~": "/local/remote",         # required
        "valueTransform": {}
      },
      {
        "receiver": "MaxLaptop",      # required
        "address": "/oschii/knob",    # required
        "type": "osc",                # "osc" | "http"
        "valueTransform": {}
      },
      {
        "*": "/oschii/knob,           # required
        "type": "osc",                # "osc" | "http"
        "valueTransform": {}
      }
    ]
  }
]
```

The `sendTo` array tells Oschii what to do with the value read from the Sensor.
Use the `"~"` syntax to send the value to a defined local Remote, or specify
a particular defined Receiver, or use the `"*"` syntax to send to ALL defined 
Receivers.

See the Receivers documentation for more details.

You can specify a `valueTransform` object on the Monitor itself and/or on 
each object in the `sendTo` array.