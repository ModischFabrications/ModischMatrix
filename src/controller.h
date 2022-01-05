#pragma once

#include "display.h"
#include "shared/serialWrapper.h"
#include "shared/network/WiFiLoginManager.h"
#include "shared/persistence/persistenceManager.h"
#include "modes/clock.h"
#include <Arduino.h>

namespace Controller {
enum Mode : uint8_t { OFF = 0, STATIC, CLOCK, WEATHER };

Mode mode;
uint32_t timeout = 0;

void setMode(uint8_t new_mode);
void printText(const String& msg);

void hideAfter(uint16_t milliseconds) { timeout = millis() + milliseconds; }

void turnOff() { Display::clear(); }

void setVisibility(bool on) {
    if (!on)
        turnOff();
}

void setBrightness(uint8_t brightness) {
    Config::Configuration config = PersistenceManager::get();
    config.brightness = brightness;
    PersistenceManager::set(config);
    // display updated from config callback
}

void updateConfig() {
    Config::Configuration config = PersistenceManager::get();
    Display::setBrightness(config.brightness);
    // TODO switch to state
}

void printText(const String& msg) {
    Display::printText(msg);
    setMode(STATIC);
}

void showLogin() {
    String msg = WiFiLoginManager::getHostname();
    msg.reserve(80);
    msg += F("\n");
    msg += WiFi.localIP().toString();
    printText(msg);
}

void setMode(uint8_t new_mode) {
    print(F("New Mode: "));
    printlnRaw(new_mode);
    switch (new_mode) {
    case OFF:
        turnOff();
        break;
    default:
        break;
    }
    Config::Configuration config = PersistenceManager::get();
    config.mode = new_mode;
    PersistenceManager::set(config);
}

void setup() {
    setVisibility(true);
    // TODO
    PersistenceManager::registerListener(updateConfig);

    Modes_Clock::setup();
}

void loop() {
    uint32_t now = millis();
    if (now > timeout && timeout != 0) {
        timeout = 0;
        setMode(OFF);
    }

    switch (mode) {
    case OFF:
    case STATIC:
    default:
        break;
    case CLOCK:
        Modes_Clock::loop();
        break;
    case WEATHER:
        // TODO pass to weather extension
        printText("Too Cold");
        break;
    }
}

} // namespace Controller
