#ifndef SensorRack_h
#define SensorRack_h

class SensorRack {
  public:
    SensorRack();
    void createSensor(JsonObject json);
    String getSensorType();
}

#endif