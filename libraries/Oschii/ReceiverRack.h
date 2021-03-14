#ifndef ReceiverRack_h
#define ReceiverRack_h

#include "Receiver.h"
#include "NetworkService.h"

#define MAX_RECEIVERS 32

class ReceiverRack {
  public:
    ReceiverRack(NetworkService * network);
    ~ReceiverRack();

    bool buildReceivers(JsonArray array);
    String buildReceiver(JsonObject json);

    Receiver * getReceiver(String name);

  private:
    NetworkService * _network;
    Receiver * _receivers[MAX_RECEIVERS];
    int _receiverIndex;

};
#endif
