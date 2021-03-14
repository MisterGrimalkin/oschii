#ifndef Receiver_h
#define Receiver_h

#include "NetworkService.h"

class Receiver {
  public:
    Receiver(NetworkService * network);

    bool build(JsonObject json);

    void sendOsc(String address, int value);

    String getName();
    String getError();
    String toString();

  private:
    NetworkService * _network;
    String _ip, _name;
    int _oscPort, _httpPort;

    void setError(String error);
    String _error;
};

#endif
