#define VERSION "0.0.2"
#define BUILD_DATETIME __DATE__ " " __TIME__

#include <Oschii.h>

Oschii oschii;

void setup() {
  Serial.begin(115200);
  oschii.start();
}

void loop() {
  oschii.loop();
}
