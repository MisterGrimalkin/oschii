#include <Arduino.h>
#include "FileService.h"
#include "SPIFFS.h"

FileService::FileService() {}

String FileService::readFile(String filename) {
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return "";
  }

  File file = SPIFFS.open(filename);

  if (!file) {
    Serial.println("There was an error opening the file for reading");
    return "";
  }

  String result = "";

  while (file.available()) {
    result += file.readString();
  }

  file.close();

  return result;
}

bool FileService::writeFile(String filename, String content) {
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return false;
  }

  File file = SPIFFS.open(filename, FILE_WRITE);

  if (!file) {
    Serial.println("There was an error opening the file for writing");
    return false;
  }
  if (!file.print(content)) {
    Serial.println("ERROR! File write failed");
  }

  file.close();

  return true;
}
