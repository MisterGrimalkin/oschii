#include <Oschii.h>

Oschii oschii;

void setup() {
  Serial.begin(115200);
  oschii.start();
}

bool on = false;

void loop() {
  oschii.loop();
}
