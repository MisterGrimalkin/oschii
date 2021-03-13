#include "Oschii.h"

Oschii::Oschii() {
  _files = new FileService();
  _settings = new SettingsService(_files, VERSION, BUILD_DATETIME);
  _racks = new Racks(_files);
  _serialAPI = new SerialAPI(_settings, _racks);
}

void Oschii::start() {
  _settings->load();
  _serialAPI->start();
  _racks->start();
}

void Oschii::loop() {
  _racks->loop();
  _serialAPI->loop();
}