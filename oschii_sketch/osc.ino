#include <ArduinoOSC.h>

void loopOsc() {
  if ( isConnected() ) OscWiFi.update();
}

void sendOsc(String host, int port, String address, int argument) {
  if ( isConnected() ) {
    if ( verbose ) {
      Serial.print("OSC ===> ");
      Serial.print(host);
      Serial.print(":");
      Serial.print(port);
      Serial.print(address);
      Serial.print(" { ");
      Serial.print(argument);
      Serial.println(" } ");
    }
    OscWiFi.send(host, port, address, argument);
  } else {
    if ( verbose ) {
      Serial.println("Disconnected! Cannot send OSC");
    }
  }
}

void sendOsc(String host, int port, String address, String payload) {
  if ( isConnected() ) {
    if ( verbose ) {
      Serial.print("OSC ===> ");
      Serial.print(host);
      Serial.print(":");
      Serial.print(port);
      Serial.print(address);
      Serial.print(" { ");
      Serial.print(payload);
      Serial.println(" } ");
    }
    OscWiFi.send(host, port, address, payload);
  } else {
    if ( verbose ) {
      Serial.println("Disconnected! Cannot send OSC");
    }
  }
}

void createOscTrigger(JsonObject outputJson) {
  Serial.println("   + Output:OSC");
  String address = outputJson["oscAddress"];

  int port = 3333;
  if ( outputJson.containsKey("oscPort") ) port = outputJson["oscPort"];

  const int sensorNo = sensorCount;
  OscWiFi.subscribe(port, address,
    [sensorNo](const OscMessage & m) {
      int size = 0;
      bool repeat = false;
      bool fade = false;
      bool cosine = false;
      int startedAt = -1;

      if ( m.size() <= 1 ) {
        int value = m.arg<int>(0);
        if ( verbose ) {
          Serial.print(value);
          Serial.println(" <--- OSC");
        }
        Sensor * sensor = &sensors[sensorNo];
        sensor->value = value;
        sensor->changed = true;

      } else {
        if ( verbose ) {
          Serial.println("(...) <--- OSC");
        }
        String type = m.arg<String>(0);
        String command = m.arg<String>(1);
        cosine = type=="cosine";
        fade = type=="linear" || cosine;
        repeat = command=="repeat";
        for ( int i = 2; i < (m.size()-1); i+=2 ) {
          int value = m.arg<int>(i);
          int time = m.arg<int>(i+1);
          patternValues[sensorNo][size] = value;
          patternTimes[sensorNo][size] = time;
          size++;
        }
        startedAt = millis();
      }
      Pattern pattern = {
        size, repeat, fade, cosine, startedAt
      };
      patterns[sensorNo] = pattern;
    }
  );

  Serial.print("     [port:");
  Serial.print(port);
  Serial.print(" address:");
  Serial.print(address);
  Serial.println("]");
}

void createOscPing() {
  OscWiFi.subscribe(OSC_PING_PORT, OSC_PING_ADDR,
  [](const OscMessage & m) {
    Serial.print("Received ping from ");
    Serial.println(m.remoteIP());
    writeToStorage("CloudIP", m.remoteIP());
    OscWiFi.send(
      m.remoteIP(),
      OSC_PING_RESPONSE_PORT,
      OSC_PING_RESPONSE_ADDR,
      ("Hello my name is: " + name).c_str()
    );
  });
}



