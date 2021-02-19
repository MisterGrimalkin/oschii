#include <WiFi.h>
#include <ETH.h>

/////////////////////
// WiFi & Ethernet //
/////////////////////

const int WIFI_TIMEOUT = 5000;
const int ETHERNET_TIMEOUT = 120000;

//static bool ethernetPreferred = false;
static bool ethernetInUse = false;
//
//String wiFiSsid     = "";
//String wiFiPassword = "";
bool connected = false;

bool isConnected() {
  return connected;
}

void setWiFiCredentials(String ssid, String password) {
  settings.setWifiSsid(ssid);
  settings.setWifiPassword(password);
}

bool isEthernetInUse() {
  return ethernetInUse;
}

String getIpAddress() {
  String ip = WiFi.localIP().toString();
  if ( isEthernetInUse() ) {
    ip = ETH.localIP().toString();
  }
  return ip;
}

String getConnectionType() {
  return ethernetInUse ? "Ethernet" : "WiFi";
}

void startNetwork() {
  if ( settings.isEthernetPreferred() ) {
    startEthernet();
  } else {
    startWiFi();
  }
}

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_ETH_START:
      ETH.setHostname(settings.getName().c_str());
      stopWiFi();
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.print("\nConnected to Ethernet on ");
      Serial.println(ETH.localIP());
      connected = true;
      ethernetInUse = true;
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("<ETH Disconnected>");
      connected = false;
      ethernetInUse = false;
      startWiFi();
      break;
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("<ETH Stopped>");
      connected = false;
      ethernetInUse = false;
      break;
    default:
      break;
  }
}

void startEthernet() {
  WiFi.onEvent(WiFiEvent);
  ETH.begin();
  Serial.print("Accessing Ethernet");
  int started = millis();
  while (!connected) {
    Serial.print(".");
    delay(500);
    if ( (millis() - started) > ETHERNET_TIMEOUT ) {
      Serial.println("\nCannot connect to Ethernet!");
      connected = false;
      ethernetInUse = false;
      startWiFi();
      return;
    }
  }
  createApi();
  server.begin();
}

void startWiFi() {
  WiFi.disconnect(true);
  String ssid = settings.getWifiSsid();
  String password = settings.getWifiPassword();

  if ( ssid != "" && password != "" ) {
    Serial.print("Accessing WiFi [" + ssid + "]");
    WiFi.begin(ssid.c_str(), password.c_str());
    int started = millis();
    while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
      delay(500);
      if ( (millis() - started) > WIFI_TIMEOUT ) {
        Serial.println("\nCannot connect to WiFi");
        connected = false;
        return;
      }
    }
    createApi();
    server.begin();
    connected = true;
    ethernetInUse = false;
    Serial.print("\nConnected to WiFi on ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("No WiFi credentials");
    connected = false;
  }
}

void stopWiFi() {
  WiFi.disconnect(true);
  connected = false;
}
