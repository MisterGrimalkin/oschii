# Receivers

_A Receiver is a device on the network to which Oschii can send OSC/HTTP
messages._

```
"receivers": [
  {
    "name": "Ada",              # required
    "ip": "192.168.1.123",      # required
    "oscPort": 3333,
    "httpPort": 80
  }
]
```

You can reference Receivers by name within the `sendTo` array of a
 Monitor definition, like so:
 
```
"sensorMonitors": [
  {
    "sensor": "Button 1"
    "sendTo": [
      {
        "receiver": "Ada",      # required
        "address": "/button1",  # required
        "type": "osc"           # "osc" | "http"
      }
    ]
  }
]
```

There is a special syntax for sending to all defined Receivers:

```
"receivers": [
  {
    "name": "Ada",
    "ip": "192.168.1.123"
  },
  {
    "name": "Bob",
    "ip": "192.168.1.100"
  }
],
"sensorMonitors": [
  {
    "sensor": "Button 1"
    "sendTo": [
      {
        "*": "/button1"
      }
    ]
  }
]
```

This will send the value from Sensor "Button 1" to the OSC address `/button1`
on both Ada AND Bob.

As explained in the Monitors documentation, you can use `"~"` in place of `"*"`
to send the value to a local Remote.