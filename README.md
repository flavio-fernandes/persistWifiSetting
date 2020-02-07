# persistWifiSetting

WiFiManager wrapper for ESP32 to be used with Arduino IDE.

This wrapper leverages the [Preferences](https://github.com/espressif/arduino-esp32/blob/master/libraries/Preferences/src/Preferences.h
) library from ESP32 to keep custom attributes in non-volatile memory:

- mqttServer
- mqttPort
- mqttUsername
- mqttPassword
- mqttTopic

At this time, there seems to be a bug in [wifiManager.getWiFiPass()](https://github.com/flavio-fernandes/WiFiManager/blob/87009dc443923721f5449e363e9aaac02da819f9/WiFiManager.h#L282)
which makes it impossible to know what is the SSID password configured. In order to work around that, the current implementation uses an extra custom field to have the password
provided (twice). ¯\\_(ツ)_/¯

Main steps:

- [Install ESP32 board in IDE](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)
- [Install WiFiManager](https://github.com/tzapu/WiFiManager). Right now, ESP32 is only supported in the [development branch](https://github.com/tzapu/WiFiManager/tree/development).
This wrapper was tested against git commit [87009dc443923721f5449e363e9aaac02da819f9](https://github.com/flavio-fernandes/WiFiManager/tree/87009dc443923721f5449e363e9aaac02da819f9).
- Copy [wifiConfig.h](https://github.com/flavio-fernandes/persistWifiSetting/blob/master/wifiConfig.h) and [wifiConfig.ino](https://github.com/flavio-fernandes/persistWifiSetting/blob/master/wifiConfig.ino) into your project
- Call `wifiConfig_init()` and `wifiConfig_get()` as shown below

In order to start the webserver portal, look at [NV_CLEAR_BUTTON_CHECK](https://github.com/flavio-fernandes/persistWifiSetting/blob/master/wifiConfig.h#L8-L12).
By default, the portal will use the ip address `http://192.168.4.1` and the SSID will be `ESP32_<UNIQUE_ID>`.

```
#include <WiFi.h>
#include "wifiConfig.h"

void setup() {
  wifiConfig_init();
  const WifiConfigData& cnf = wifiConfig_get();

  WiFi.mode(WIFI_STA);
  WiFi.begin(cnf.wifiSsid.c_str(), cnf.wifiPass.c_str());
}

void loop() {
  // do something amazing here...
}
```

