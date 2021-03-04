#include "SerialAPI.h"

SerialAPI::SerialAPI(Racks * racks) {
  _racks = racks;
}

void SerialAPI::start() {
  Serial.println();
  Serial.println();
  Serial.println("  ╔═╗┌─┐┌─┐┬ ┬┬┬");
  Serial.println("  ║ ║└─┐│  ├─┤││");
  Serial.println("  ╚═╝└─┘└─┘┴ ┴┴┴");
//  Serial.print  ("  ");
//  Serial.println(VERSION);
//  Serial.println();
//  Serial.print  ("  ");
//  Serial.println(settings.getName());
//  Serial.println();
//  Serial.print  ("  Built on ");
//  Serial.println(BUILD_DATETIME);
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

  } else if ( input == "set config" ) {
    String jsonStr = prompt("Ready for config:");
    _racks->buildConfig(jsonStr);

  } else if ( input == "set scene" ) {
    String jsonStr = prompt("Ready for scene:");
    _racks->buildScene(jsonStr);

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
