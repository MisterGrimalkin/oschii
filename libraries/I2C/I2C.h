#ifndef I2C_h
#define I2C_h

class I2C {
  public:
    I2C();
    void start_gpio(int sda_pin, int scl_pin);
    int read_gpio(int device_address, int register_address);
    void write_gpio(int device_address, int register_address, int data);
};

#endif