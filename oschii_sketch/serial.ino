void startSerial() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println("  ╔═╗┌─┐┌─┐┬ ┬┬┬");
  Serial.println("  ║ ║└─┐│  ├─┤││");
  Serial.println("  ╚═╝└─┘└─┘┴ ┴┴┴");
  Serial.print  ("  ");
  Serial.println(VERSION);
  Serial.println();
  Serial.print  ("  ");
  Serial.println(name);
  Serial.println();
  Serial.print  ("  Built on ");
  Serial.println(BUILD_DATETIME);
  Serial.println();
}

void processSerialInput(String input) {
  if ( input == "poke" ) {
    Serial.println("Tickles!");

  } else if ( input == "name" ) {
    Serial.println(name);

  } else if ( input == "set name" ) {
    name = promptSerial("Ready for name");
    writeToStorage("name", name);
    Serial.print("Name is now ");
    Serial.println(name);

  } else if ( input == "ip" ) {
    if ( isConnected() ) {
      Serial.print(getIpAddress());
      Serial.print(" (");
      Serial.print(getConnectionType());
      Serial.println(")");
    } else {
      Serial.println("DISCONNECTED");
    }

  } else if ( input == "ssid" ) {
    Serial.println(getWiFiSsid());

  } else if ( input == "start wifi" ) {
    writeToStorage("enableEthernet", "no");
    String ssid = promptSerial("Ready for ssid");
    writeToStorage("ssid", ssid);
    String password = promptSerial("Ready for password");
    writeToStorage("password", password);
    setWiFiCredentials(ssid, password);
    startWiFi();

  } else if ( input == "start ethernet" ) {
    stopWiFi();
    setEthernetPreferred(true);
    startEthernet();
    writeToStorage("enableEthernet", "yes");

  } else if ( input == "stop ethernet" ) {
    stopWiFi();
    setEthernetPreferred(false);
    writeToStorage("enableEthernet", "no");
    reboot = true;

  } else if ( input == "config" ) {
    Serial.println(readFile("/config.json"));

  } else if ( input == "set config" ) {
    String config = promptSerial("Ready for config");
    Serial.println(parseJson(config));;
  }
}

void loopSerial() {
  String input = readSerial();
  if ( input != "" ) {
    processSerialInput(input);
  }
}

String promptSerial(String prompt) {
  Serial.println(prompt);
  while (!Serial.available());
  return readSerial();
}

String readSerial() {
  if ( Serial.available() ) {
    return Serial.readString();
  }
  return "";
}

