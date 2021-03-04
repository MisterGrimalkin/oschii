# Monitors

_A Monitor watches a Sensor. When the Sensor's value changes
(and/or at regular polling intervals) the Monitor will send
the value to the defined network Receivers, and to the
defined Remotes._

##### Configuration:
```
"sensorMonitors": {
  "SENSOR NAME": {
    "onChange": true,
    "pollInterval": 250,
    "reportTo": [
      {
        "receivers": [
          "Ada", "Bob", ...
        ],
        "address": "/endpoint",
        "type": "osc"
      },
      ...
    ],
    "fireRemotes": [
      "/remote1", "/remote2", ...
    ]
  }
}
```