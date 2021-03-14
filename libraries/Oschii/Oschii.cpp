#include "Oschii.h"

Oschii::Oschii() {
  _files = new FileService();
  _settings = new SettingsService(_files, VERSION, BUILD_DATETIME);
  _network = new NetworkService(_settings);
  _racks = new Racks(_files, _network);
  _serialAPI = new SerialAPI(_settings, _network, _racks);
}

void Oschii::start() {
  _settings->load();
  _serialAPI->start();
  _network->connect();
  _racks->start();
}

void Oschii::loop() {
  _serialAPI->loop();
  _network->loop();
  _racks->loop();
}