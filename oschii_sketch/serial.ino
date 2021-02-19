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
  Serial.println(settings.getName());
  Serial.println();
  Serial.print  ("  Built on ");
  Serial.println(BUILD_DATETIME);
  Serial.println();
}

void processSerialInput(String input) {
  if ( input == "poke" ) {
    Serial.println("Tickles!");

  } else if ( input == "setup" ) {
    Serial.println(settings.toJson());

  } else if ( input == "name" ) {
    Serial.println(settings.getName());

  } else if ( input == "set name" ) {
    String name = promptSerial("Ready for name");
    settings.setName(name);
    Serial.print("Name is now ");
    Serial.println(settings.getName());

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
    Serial.println(settings.getWifiSsid());

  } else if ( input == "start wifi" ) {
    settings.setEthernetPreferred(false);

    String ssid = promptSerial("Ready for ssid");
    settings.setWifiSsid(ssid);

    String password = promptSerial("Ready for password");
    settings.setWifiPassword(password);

    startWiFi();

  } else if ( input == "start ethernet" ) {
    stopWiFi();
    settings.setEthernetPreferred(true);
    startEthernet();

  } else if ( input == "stop ethernet" ) {
    stopWiFi();
    settings.setEthernetPreferred(false);
    reboot = true;

  } else if ( input == "config" ) {
    Serial.println(files.readFile("/config.json"));

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

