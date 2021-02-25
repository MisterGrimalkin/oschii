#ifndef BinarySensor_h
#define BinarySensor_h

class BinarySensor : public OSensor {
  public:
    BinarySensor() : Sensor() {};
    BinarySensor(int index) : Sensor(index) {};

    virtual void readSensor();

    virtual bool build(JsonObject json);
    virtual void print() {};
    virtual int getReading() {};

  protected:
    int _bounceFilter, _holdFilter, _onValue, _offValue;
    bool _invert;
};

#endif