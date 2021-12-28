#pragma once

#include <WiFiManager.h> // captive portal for wifi
// TODO include guard for ESP8266/ESP32
#include <ESPmDNS.h>

#include "shared/SerialWrapper.h"

namespace WiFiLoginManager {

namespace {
WiFiManager wifiManager;
}

// gets called when WiFiManager enters configuration mode
void configModeCallback(WiFiManager* myWiFiManager) {
    println(F("Entered config mode"));
    // auto generated SSID might be unknown
    printlnRaw(myWiFiManager->getConfigPortalSSID());
    printlnRaw(WiFi.softAPIP().toString());
}

void setup(const char* name) {
    println(F("setting up wifi (with captive portal)"));

    // DEBUGGING:
    // wifiManager.resetSettings();

    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1),
                                    IPAddress(255, 255, 255, 0));
    // wifiManager.setConfigPortalTimeout(60);

    // LAN name if successful
    WiFi.hostname(name);

    // hotspot name if not
    if (!wifiManager.autoConnect(name)) {
        logError(F("failed to connect and hit timeout"));
        // reset and try again after waking up
        delay(1000);
        ESP.restart();
    }

    // if you get here you have connected to the WiFi
    println(F("connected to the local network"));

    // TODO register MDNS
    if (!MDNS.begin(name)) { // Start the mDNS responder
        logWarning(F("Error setting up mDNS responder!"));
    }
}

} // namespace WiFiLoginManager
