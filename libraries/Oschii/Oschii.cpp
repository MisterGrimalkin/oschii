#include "Oschii.h"

Oschii::Oschii() {
  _racks = new Racks();
  _serial = new SerialAPI(_racks);
}

void Oschii::start() {
  _racks->start();
  _serial->start();
}

void Oschii::loop() {
  _racks->loop();
  _serial->loop();
}