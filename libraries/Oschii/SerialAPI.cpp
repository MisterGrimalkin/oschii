#include "SerialAPI.h"

SerialAPI::SerialAPI(SettingsService * settings, Racks * racks) { //}, NetworkService * network) {
  _settings = settings;
  _racks = racks;
//  _network = network;
}

void SerialAPI::start() {
  Serial.println();
  Serial.println();
  Serial.println("  ╔═╗┌─┐┌─┐┬ ┬┬┬");
  Serial.println("  ║ ║└─┐│  ├─┤││");
  Serial.println("  ╚═╝└─┘└─┘┴ ┴┴┴");
  Serial.print  ("  ");
  Serial.println(_settings->getVersion());
  Serial.println();
  Serial.print  ("  ");
  Serial.println(_settings->getName());
  Serial.println();
  Serial.print  ("  Built on ");
  Serial.println(_settings->getBuildDatetime());
  Serial.println();
}

void SerialAPI::loop() {
  String input = read();
  if ( input != "" ) {
    processInput(input);
  }
}

void SerialAPI::processInput(String input) {
  if ( input == "poke" ) {
    Serial.println("Tickles!");

  } else if ( input == "version" ) {
    Serial.print(_settings->getVersion());
    Serial.print(" - Built on ");
    Serial.println(_settings->getBuildDatetime());

  } else if ( input == "settings" ) {
    Serial.println(_settings->toPrettyJson());

  } else if ( input == "settings=" ) {
    String settings = prompt(">> Enter new settings <<\n");
    _settings->set(settings);
    Serial.println(_settings->toPrettyJson());

  } else if ( input == "name" ) {
    Serial.println(_settings->getName());

  } else if ( input == "name=" ) {
    String name = prompt(">> Enter new name <<\n");
    _settings->setName(name);
    Serial.print("Name is now ");
    Serial.println(_settings->getName());

//  } else if ( input == "config" ) {
//    Serial.println(_racks->toPrettyJson());

  } else if ( input == "config=" ) {
    String configStr = prompt(">> Enter new configuration <<\n");
    _racks->buildConfig(configStr);

  }
}

String SerialAPI::prompt(String prompt) {
  Serial.println(prompt);
  while (!Serial.available());
  return read();
}

String SerialAPI::read() {
  if ( Serial.available() ) {
    return Serial.readString();
  }
  return "";
}
