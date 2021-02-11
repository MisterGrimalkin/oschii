const int DEVICES_LIMIT = 64;

struct Device {
  String name;
  String ip;
};

Device devices[DEVICES_LIMIT];
int deviceCount = 0;

int getDeviceCount() {
  return deviceCount;
}

String buildDevices(JsonArray devicesJson) {
  deviceCount = 0;
  for ( int i = 0; i < DEVICES_LIMIT; i++ ) {
    JsonObject deviceJson = devicesJson[i];
    if ( deviceJson.containsKey("name") ) {
      if ( deviceJson.containsKey("ip") ) {
        String name = deviceJson["name"];
        String ip = deviceJson["ip"];
        Serial.print("   Device ");
        Serial.print(name);
        Serial.print(" at ");
        Serial.println(ip);
        devices[deviceCount++] = { deviceJson["name"], deviceJson["ip"] };
      } else {
        String name = deviceJson["name"];
        return "ERROR! Missing IP for Device '" + name + "'";
      }
    }
  }
  return "OK";
}

String getDeviceIp(String name) {
  if ( name == "_CLOUD_" ) {
    return readFromStorage("CloudIP");
  }
  int i;
  for ( i = 0; i < deviceCount; i++ ) {
    Device device = devices[i];
    if ( device.name == name ) {
      return device.ip;
    }
  }
  return "";
}
