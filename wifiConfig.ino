#include "wifiConfig.h"

#include <Preferences.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#include <Esp.h>

#define DEFAULT_WIFIMANAGER_PORTAL_TIMEOUT 300  /* 5 minutes, in seconds */
#define ESP_BOARD_LED 13  // https://learn.adafruit.com/adafruit-huzzah32-esp32-feather?view=all#pinouts
static const size_t _ATTR_MAX_LEN = 40;
static const char* const PREFERENCES_NAME = "myPrefs";

static const char* const ATTR_WIFI_SSID = "wifi_ssid";
static const char* const ATTR_WIFI_PASS = "wifi_pass";

static const String attrValidSave("valid_save");  // used as key and value

static WifiConfigData _wifiConfigData;
static bool _wifiConfigDataLoaded = false;

static bool _shouldResetNv() {
#ifdef NV_CLEAR_BUTTON_CHECK
  pinMode(NV_CLEAR_BUTTON_CHECK, INPUT);
  return digitalRead(NV_CLEAR_BUTTON_CHECK) == HIGH;
#else
  return false;
#endif  // fdef NV_CLEAR_BUTTON_CHECK
}


static void _kaboom() {
#ifdef ESP_BOARD_LED
  digitalWrite(ESP_BOARD_LED, HIGH);
#endif
  delay(5000);
  ESP.restart();
}


const WifiConfigData& wifiConfig_get() {
  if (!_wifiConfigDataLoaded) {
#ifdef DEBUG
    Serial.printf("wifiConfig_get called before wifiConfig_init. BOOM!\n");
#endif
    _kaboom();
  }
  return _wifiConfigData;
}


void  wifiConfig_init() {
#ifdef DEBUG
  Serial.printf("wifiConfig_init called\n");
#endif
  
#ifdef ESP_BOARD_LED
  pinMode(ESP_BOARD_LED, OUTPUT);
  digitalWrite(ESP_BOARD_LED, LOW);
#endif

  Preferences preferences;
  preferences.begin(PREFERENCES_NAME /*name*/, false /*readOnly*/);

  if (_shouldResetNv()) {
#ifdef DEBUG
    Serial.printf("wifiConfig resetting memory content and restarting ESP\n");
#endif
    {
      WiFiManager wifiManager;
      wifiManager.resetSettings();
    }
    preferences.clear();  // erase settings

    preferences.end(); _kaboom();
  }

  //char wifi_ssid[_ATTR_MAX_LEN] = {0}; preferences.getString(ATTR_WIFI_SSID, wifi_ssid, sizeof(wifi_ssid));
  char wifi_pass[_ATTR_MAX_LEN] = {0}; preferences.getString(ATTR_WIFI_PASS, wifi_pass, sizeof(wifi_pass));

  // Use attrValidSave as the flag to indicate whether we need to kick off WiFiManager portal or not
  if (true ||  // HACK TO FORCE PORTAL TO START EVERY TIME AND REPODUCE BUG
      preferences.getString(attrValidSave.c_str()) != attrValidSave) {
#ifdef ESP_BOARD_LED
    digitalWrite(ESP_BOARD_LED, HIGH);
#endif

    WiFiManager wifiManager;
    wifiManager.setConfigPortalTimeout(DEFAULT_WIFIMANAGER_PORTAL_TIMEOUT);
    wifiManager.setBreakAfterConfig(true /*shouldBreak*/);

    //WiFiManagerParameter custom_wifi_ssid(ATTR_WIFI_SSID, "ssid (to be saved)", wifi_ssid, sizeof(wifi_ssid));
    WiFiManagerParameter custom_wifi_pass(ATTR_WIFI_PASS, "ssid password (to be saved)", wifi_pass, sizeof(wifi_pass));

    //wifiManager.addParameter(&custom_wifi_ssid);
    wifiManager.addParameter(&custom_wifi_pass);

    wifiManager.setDebugOutput(true /*debug*/);
    
    // NOTE: Instead of looking at result from startConfigPortal, simply use wifiManager.getWiFiSSID
    // as a parameter to indicate failure in general.
    if (!wifiManager.startConfigPortal()) {
      Serial.printf("wifiManager hit a snag\n");
      preferences.end(); _kaboom();
    }

    preferences.putString(ATTR_WIFI_SSID, wifiManager.getWiFiSSID(true /*persistent*/));
    preferences.putString(ATTR_WIFI_PASS, wifiManager.getWiFiPass(true /*persistent*/));
#ifdef DEBUG
    Serial.printf("wifiManager id: %s value: %s\n", ATTR_WIFI_SSID, preferences.getString(ATTR_WIFI_SSID).c_str());
    Serial.printf("**BUG** wifiManager id: %s value: %s\n", ATTR_WIFI_PASS, preferences.getString(ATTR_WIFI_PASS).c_str());
    Serial.printf("**BUG** wifiManager id: %s value: %s\n", ATTR_WIFI_PASS, wifiManager.getWiFiPass(true).c_str());
    Serial.printf("**BUG** wifiManager id: %s value: %s\n", ATTR_WIFI_PASS, wifiManager.getWiFiPass(false).c_str());
#endif

    WiFiManagerParameter** parameters = wifiManager.getParameters();
    int parametersCount = wifiManager.getParametersCount();
    for (int i = 0; i < parametersCount; ++i) {
      /*const*/ WiFiManagerParameter& p = *parameters[i];
#ifdef DEBUG
      Serial.printf("id: %s value: %s valueLen: %d label: %s\n",
		    p.getID(), p.getValue(), p.getValueLength(), p.getLabel());
#endif
      preferences.putString(p.getID(), p.getValue());
    }

    // last but not least: save the fact that we got all the ducks in a row (aka persisted params set)
    preferences.putString(attrValidSave.c_str(), attrValidSave);

#ifdef DEBUG
    Serial.printf("wifiManager parameters saved as preferences\n");
#endif

    preferences.end(); _kaboom();
  }

#ifdef DEBUG
  Serial.printf("%s: %s\n", ATTR_WIFI_SSID, preferences.getString(ATTR_WIFI_SSID).c_str());
  Serial.printf("%s: %s\n", ATTR_WIFI_PASS, preferences.getString(ATTR_WIFI_PASS).c_str());
  Serial.printf("%s: %s\n", attrValidSave.c_str(), preferences.getString(attrValidSave.c_str()).c_str());
#endif

  // fill in wifiConfigData
  _wifiConfigData.wifiSsid = preferences.getString(ATTR_WIFI_SSID);
  _wifiConfigData.wifiPass = preferences.getString(ATTR_WIFI_PASS);

  _wifiConfigDataLoaded = true;
  preferences.end();
}
