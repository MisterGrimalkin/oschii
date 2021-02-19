#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>

const char *  HTTP_CONFIG_ENDPOINT = "/config";
const char *  HTTP_NAME_ENDPOINT = "/name";

AsyncWebServer server(80);

void sendHttp(String method, String url) {
  sendHttp(method, url, "HELLO");
}

void sendHttp(String method, String url, String payload) {
  if (isConnected()) {
    if ( verbose ) {
      Serial.print("HTTP ===> ");
      Serial.print(method);
      Serial.print(" ");
      Serial.print(url);
      Serial.print(" { ");
      Serial.print(payload);
      Serial.print(" } ");
    }

    HTTPClient http;
    http.begin(url);

    int httpResponseCode;
    if ( method == "post" ) {
      httpResponseCode = http.POST(payload);
    } else if ( method == "put" ) {
      httpResponseCode = http.PUT(payload);
    } else {
      httpResponseCode = http.GET();
    }

    if (httpResponseCode > 0) {
      if ( verbose ) {
        Serial.print("OK: ");
        Serial.println(httpResponseCode);
      }
    }
    else {
      if ( verbose ) {
        Serial.print("Error: ");
        Serial.println(httpResponseCode);
      }
    }
    http.end();
  }
  else {
    if ( verbose ) Serial.println("Cannot Send HTTP - Network Disconnected");
  }
}

void createHttpApi() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    String response =
    "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"utf-8\"></head>"
    "<body style='background-color:#068800;color:white;padding:10px;'>"
      "<p style='font-family:courier;color:white;font-size:large'>"
        "<strong>╔═╗┌─┐┌─┐┬<span style='color:#068800'>─</span>┬┬┬<br>"
        "║<span style='color:#068800'>─</span>║└─┐│<span style='color:#068800'>──</span>├─┤││<br>"
        "╚═╝└─┘└─┘┴<span style='color:#068800'>─</span>┴┴┴</strong><br>"
        VERSION "<br>"
       "<p style='font-family:sans-serif;font-weight:bold;font-size:x-large;color:white'>"
        + String(settings.getName()) +
       "<p style='font-family:sans-serif;font-size: small;color:white'>"
        "Built on <strong>" BUILD_DATETIME "<strong><br>"
    "</body></html>";
    request->send(200, "text/html", response);
  });

  server.on(HTTP_NAME_ENDPOINT, HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", settings.getName());
  });

  server.on(HTTP_NAME_ENDPOINT, HTTP_POST, [](AsyncWebServerRequest * request) {
    String name = request->getParam("body", true)->value();
    settings.setName(name);
    String reply = "Name is now " + settings.getName() + "\n";
    request->send(200, "text/plain", reply);
  }, NULL, [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {});

  server.on(HTTP_CONFIG_ENDPOINT, HTTP_GET, [](AsyncWebServerRequest * request) {
    String config = files.readFile("/config.json");
    request->send(200, "text/plain", config);
  });

  server.on(HTTP_CONFIG_ENDPOINT, HTTP_POST, [](AsyncWebServerRequest * request) {
    String reply = parseJson(request->getParam("body", true)->value());
    int ok = (reply == CONFIG_OK);
    request->send(ok ? 200 : 400, "text/plain", reply);
    reboot = true;
  }, NULL, [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {});
}

void createHttpTrigger(JsonObject outputJson) {
  Serial.println("   + Output:HTTP");
  const char * path = outputJson["httpPath"].as<char*>();

  WebRequestMethod method;
  String methodStr = "post";
  if ( outputJson.containsKey("httpMethod") ) outputJson["httpMethod"];
  if ( methodStr == "post" ) {
    method = HTTP_POST;
  } else if ( methodStr == "put" ) {
    method = HTTP_PUT;
  } else if ( methodStr == "delete" ) {
    method = HTTP_DELETE;
  } else {
    method = HTTP_GET;
  }
  const int sensorNo = sensorCount;
  server.on(path, method, [sensorNo](AsyncWebServerRequest * request) {
    String valueStr = request->getParam("body", true)->value();
    int value = valueStr.toInt();
    Serial.print(value);
    Serial.println(" <--- HTTP");
    Sensor * sensor= &sensors[sensorNo];
    sensor->value = value;
    sensor->changed = true;
    request->send(200, "text/plain", "OK");
  }, NULL, [](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {});

  Serial.print("     [method:");
  Serial.print(methodStr);
  Serial.print(" path:");
  Serial.print(path);
  Serial.println("]");
}

