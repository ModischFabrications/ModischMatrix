#pragma once

#include "display.h"
#include "modes/clock.h"
#include "shared/network/WiFiLoginManager.h"
#include "shared/persistence/persistenceManager.h"
#include "shared/serialWrapper.h"
#include <Arduino.h>

namespace Controller {
enum Mode : uint8_t { OFF = 0, STATIC, LOGIN, CLOCK, WEATHER, SIZE };

void turnOff();
void setMode(uint8_t new_mode);
void printText(const String& msg);
void showLogin();

namespace {
Mode mode;
uint32_t timeout = 0;

void _toMode(Mode new_mode) {
    if (new_mode == mode)
        return;
    print(F("New Mode: "));
    switch (new_mode) {
    case OFF:
        println(F("OFF"));
        turnOff();
        break;
    case STATIC:
        println(F("STATIC"));
        break;
    case LOGIN:
        println(F("LOGIN"));
        showLogin();
        break;
    case CLOCK:
        println(F("CLOCK"));
        Display::clear();
        break;
    case WEATHER:
        println(F("WEATHER"));
        Display::clear();
        break;
    default:
        println(F("UNKNOWN, discarding..."));
        _toMode(OFF);
        return;
    }
    mode = new_mode;
}
} // namespace

void hideAfter(uint16_t milliseconds) { timeout = millis() + milliseconds; }

void turnOff() {
    Display::clear();
    setMode(OFF);
}

void setBrightness(uint8_t brightness) {
    Config::Configuration config = PersistenceManager::get();
    config.brightness = brightness;
    PersistenceManager::set(config);
    // display updated from config callback
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
    Display::clear();
    Display::printText(msg);
    setMode(LOGIN);
}

void setMode(uint8_t new_mode) {
    if (new_mode >= SIZE) {
        print(F("New Mode "));
        printRaw(new_mode);
        println(F(" is out of bounds!"));
        return;
    }

    Config::Configuration config = PersistenceManager::get();
    config.mode = new_mode;
    PersistenceManager::set(config);

    // mode updated from config callback
}

void updateConfig() {
    println(F("C: Loading config..."));
    Config::Configuration config = PersistenceManager::get();
    Display::setBrightness(config.brightness);
    _toMode((Mode)config.mode);
}

void setup() {
    PersistenceManager::registerListener(updateConfig);

    Modes_Clock::setup();
}

void loop() {
    uint32_t now = millis();
    if (timeout != 0 && now > timeout) {
        println(F("C: Timeout"));
        timeout = 0;
        setMode(OFF);
    }

    switch (mode) {
    default:
        break;
    case CLOCK:
        Modes_Clock::loop();
        break;
    case WEATHER:
        // TODO pass to weather extension
        Display::printText("Too Cold!\n (TODO)");
        delay(500);
        break;
    }
}

} // namespace Controller
