#pragma once

#include "matrix/display.h"
#include "shared/network/WiFiLoginManager.h"
#include "shared/persistence/persistenceManager.h"
#include <Arduino.h>

namespace Controller {
bool visible = true;

enum Mode { OFF, IDLE, CLOCK, WEATHER };

Mode mode;
uint32_t timeout = 0;

void hideAfter(uint16_t milliseconds) { timeout = millis() + milliseconds; }

void turnOff() {
    visible = false;
    Display::flashBorder();
    Display::clear();
}

void setVisibility(bool on) {
    if (!on)
        turnOff();
}

void printText(const String& msg) {
    if (!visible)
        return;
    Display::printText(msg);
}

void showLogin() {
    String msg = WiFiLoginManager::getHostname();
    msg.reserve(80);
    msg += F("\n");
    msg += WiFi.localIP().toString();
    printText(msg);
}

void setMode(uint8_t new_mode) {
    if (new_mode == OFF) {
        ;
        return;
    }
    visible = true;
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

void setBrightness(uint8_t brightness) {
    // TODO update display
    Config::Configuration config = PersistenceManager::get();
    config.brightness = brightness;
    PersistenceManager::set(config);
}

void updateConfig() {
    Config::Configuration config = PersistenceManager::get();
    Display::setBrightness(config.brightness);
}

void setup() {
    setVisibility(true);
    // TODO
    PersistenceManager::registerListener(updateConfig);
}

void loop() {
    uint32_t now = millis();
    if (now > timeout && timeout != 0) {
        timeout = 0;
        setMode(OFF);
    }

    switch (mode) {
    case OFF:
    case IDLE:
    default:
        break;
    case CLOCK:
        // TODO pass to clock extension
        printText("04:20");
        break;
    case WEATHER:
        // TODO pass to weather extension
        printText("Too Cold");
        break;
    }
}

} // namespace Controller
