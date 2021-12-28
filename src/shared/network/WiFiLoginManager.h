#pragma once

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
// TODO include guard for ESP8266/ESP32?
#include <ESP32Ping.h> // https://github.com/marian-craciunescu/ESP32Ping
#include <ESPmDNS.h>

#include "shared/SerialWrapper.h"

namespace WiFiLoginManager {

namespace {
// IPAddress testHost(8, 8, 8, 8);
IPAddress testHost(1, 1, 1, 1);
WiFiManager wifiManager;
} // namespace

// gets called when WiFiManager enters configuration mode
void configModeCallback(WiFiManager* myWiFiManager) {
    println(F("Entered config mode, connect manually to: "));
    // auto generated SSID might be unknown
    printlnRaw(myWiFiManager->getConfigPortalSSID());
    printlnRaw(WiFi.softAPIP().toString());
}

void setupWiFi(const char* name) {
    println(F("setting up wifi (with captive portal)"));

    // DEBUGGING:
    // wifiManager.resetSettings();

    wifiManager.setAPCallback(configModeCallback);
    // wifiManager.setConnectTimeout(5);
    wifiManager.setMinimumSignalQuality(10);
    wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1),
                                    IPAddress(255, 255, 255, 0));

    // LAN name if successfully connected
    WiFi.hostname(name);

    // try connecting, show captive portal on fail
    if (!wifiManager.autoConnect(name)) {
        logError(F("failed to connect and hit timeout"));
        // reset and try again after waking up
        delay(1000);
        ESP.restart();
    }

    // if you get here you have connected to the WiFi
    println(F("connected to the local network"));
}

void setupMDNS(const char* name) {
    // TODO register MDNS
    if (!MDNS.begin(name)) { // Start the mDNS responder
        logWarning(F("Error setting up mDNS responder!"));
    }
    // TODO? MDNS.addService("http", "tcp", 80);
    println(F("mDNS ready"));
}

void testConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        logError(F("WiFi seems to be disconnected!"));
        return;
    }
    print(F("Testing connection to "));
    printlnRaw(testHost.toString());
    if (!Ping.ping(testHost, 4)) {
        logError(F("Can't connect to anything"));
        return;
    }
    print(F("RTT: "));
    printlnRaw(Ping.averageTime());

    println(F("All good!"));
}

void setup(const char* name) {
    setupWiFi(name);
    setupMDNS(name);
    testConnection();
}

} // namespace WiFiLoginManager
