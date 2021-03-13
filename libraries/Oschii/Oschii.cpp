#include "Oschii.h"

Oschii::Oschii() {
  _files = new FileService();
  _settings = new SettingsService(_files, VERSION, BUILD_DATETIME);
  _racks = new Racks();
  _serialAPI = new SerialAPI(_settings, _racks);
}

void Oschii::start() {
  _settings->load();
  _racks->start();
  _serialAPI->start();
}

void Oschii::loop() {
  _racks->loop();
  _serialAPI->loop();
}