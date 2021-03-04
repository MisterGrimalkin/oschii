#ifndef SerialAPI_h
#define SerialAPI_h

#include <Arduino.h>

#include "Racks.h"

class SerialAPI {
  public:
    SerialAPI(Racks * racks);

    void start();
    void loop();

    void processInput(String input);

    String prompt(String prompt);
    String read();

  private:
    Racks * _racks;
};

#endif