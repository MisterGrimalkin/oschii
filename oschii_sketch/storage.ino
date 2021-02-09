#include <Preferences.h>

////////////
// EEPROM //
////////////

const char * PREFS_NAME = "oschii";

Preferences prefs;

void writeToStorage(String key, String value) {
  prefs.begin(PREFS_NAME, false);
  prefs.putString(key.c_str(), value);
  prefs.end();
}

String readFromStorage(String key) {
  prefs.begin(PREFS_NAME, true);
  String value = prefs.getString(key.c_str());
  prefs.end();
  return value;
}


////////////
// SPIFFS //
////////////

bool writeFile(String filename, String content) {
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

String readFile(String filename) {
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
