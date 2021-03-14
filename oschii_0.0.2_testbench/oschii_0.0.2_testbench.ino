#include <Oschii.h>

Oschii * oschii;

void setup() {
  Serial.begin(115200);
  (oschii = new Oschii())->start();
}

void loop() {
  oschii->loop();
}
