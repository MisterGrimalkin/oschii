#include "NetworkService.h"

NetworkService::NetworkService(SettingsService * settings) {
  _settings = settings;
  _connected = false;
  _usingEthernet = false;
}

void NetworkService::connect() {
  if ( _connected ) {
    disconnect();
  }

  Serial.print("> Network: ");

  if ( _settings->isEthernetEnabled() ) {
    Serial.println("ETHERNET");
//    connectToEthernet();
  } else if ( _settings->isWifiEnabled() ) {
    Serial.println("WIFI");
    connectToWifi();
  } else {
    Serial.println("(none)");
  }

  Serial.println();
}

void NetworkService::disconnect() {
  if ( _connected && !_usingEthernet ) {
    WiFi.disconnect(true);
    _connected = false;
  }
}

void NetworkService::loop() {
}

bool NetworkService::isConnected() {
  return _connected;
}

String NetworkService::getIpAddress() {
  if ( _connected ) {
    if ( _usingEthernet ) {
      return ETH.localIP().toString();
    } else {
      return WiFi.localIP().toString();
    }
  } else {
    return "NOT CONNECTED";
  }
}

//static void WiFiEvent(WiFiEvent_t event) {
//  switch (event) {
//    case SYSTEM_EVENT_ETH_START:
//      ETH.setHostname(_settings->getName().c_str());
//      WiFi.disconnect(true);
//      break;
//    case SYSTEM_EVENT_ETH_CONNECTED:
//      break;
//    case SYSTEM_EVENT_ETH_GOT_IP:
//      Serial.print("\nConnected to Ethernet on ");
//      Serial.println(ETH.localIP());
//      _connected = true;
//      _usingEthernet = true;
//      break;
//    case SYSTEM_EVENT_ETH_DISCONNECTED:
//      Serial.println("<ETH Disconnected>");
//      _connected = false;
//      _usingEthernet = false;
//      connectToWifi();
//      break;
//    case SYSTEM_EVENT_ETH_STOP:
//      Serial.println("<ETH Stopped>");
//      _connected = false;
//      _usingEthernet = false;
//      break;
//    default:
//      break;
//  }
//}
//
//void NetworkService::connectToEthernet() {
//  WiFi.onEvent(WiFiEvent);
//  ETH.begin();
//  Serial.print("Accessing Ethernet");
//  int started = millis();
//  int timeout = _settings->getEthernetTimeout();
//  while ( !_connected ) {
//    Serial.print(".");
//    delay(500);
//    if ( (millis() - started) > timeout ) {
//      Serial.println("\nCannot connect to Ethernet!");
//      _connected = false;
//      _usingEthernet = false;
//      connectToWifi();
//      return;
//    }
//  }
//}

void NetworkService::connectToWifi() {
  WiFi.disconnect(true);
  String ssid = _settings->getWifiName();
  String password = _settings->getWifiPassword();
  if ( ssid != "" && password != "" ) {
    Serial.print("  Accessing WiFi [" + ssid + "]");
    WiFi.begin(ssid.c_str(), password.c_str());
    int started = millis();
    int timeout = _settings->getWifiTimeout();
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
      if ( (millis() - started) > timeout ) {
        Serial.println("\n  Cannot connect to WiFi");
        _connected = false;
        return;
      }
    }
    _connected = true;
    _usingEthernet = false;
    Serial.print("\n  Connected to WiFi on ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("  No WiFi credentials");
    _connected = false;
  }
}
