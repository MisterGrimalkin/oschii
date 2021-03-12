#include <Oschii.h>

Oschii oschii;

void setup() {
  Serial.begin(115200);
  oschii.start();
}

void loop() {
  oschii.loop();
}
