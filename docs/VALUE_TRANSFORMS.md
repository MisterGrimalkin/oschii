# Value Transforms

The four main objects in the configuration document can all accept a `valueTransform` object, which is applied
as follows:

- **Sensor**: [reading from physical sensor] **-->** [value sent to Monitor]
- **Monitor**: [value from Sensor] **-->** [value sent to Remote or network]
- **Remote**: [value from Monitor or network] **-->** [value sent to Driver]
- **Driver**: [value from Remote] **-->** [setting written to physical driver]

```
"valueTransform": {
  "inputRange": [
    0,
    4095
  ],
  "discardOutliers": -1,
  
  "outputRange": [
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
  ]
}
```


