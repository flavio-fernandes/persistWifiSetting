//#define DEBUG 1

#include <WiFi.h>
#include "wifiConfig.h"

void setup() {
  Serial.begin(115200);

  Serial.println("here we go");
  wifiConfig_init();
  const WifiConfigData& cnf = wifiConfig_get();

  Serial.printf("ssid %s pass %s\n", cnf.wifiSsid.c_str(), cnf.wifiPass.c_str());
  Serial.printf("mqttServer %s mqttPort %u\n", cnf.mqttServer.c_str(), (unsigned int) cnf.mqttPort);
  Serial.printf("mqttUsername %s mqttPassword %s\n", cnf.mqttUsername.c_str(), cnf.mqttPassword.c_str());
  Serial.printf("mqttTopic prefix %s\n", cnf.mqttTopic.c_str());

  WiFi.mode(WIFI_STA);
  WiFi.begin(cnf.wifiSsid.c_str(), cnf.wifiPass.c_str());
}

void loop() {
  static int lassConnected = WL_IDLE_STATUS;

  const int currConnected = WiFi.status();
  if (currConnected != lassConnected) {
    char statusBuff[10]; snprintf(statusBuff, sizeof(statusBuff),"%d", currConnected);
    Serial.printf("WiFi %s\n", currConnected == WL_CONNECTED ? "connected" : statusBuff);
    if (currConnected == WL_CONNECTED) {
      Serial.print("Local IP: "); Serial.println(WiFi.localIP());
      Serial.print("signal strength (RSSI): "); Serial.println(WiFi.RSSI());
    }
    lassConnected = currConnected;
  }

  delay(3000);

}
