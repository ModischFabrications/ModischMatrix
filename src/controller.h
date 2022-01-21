#pragma once

#include "display.h"

#include "modes/clock.h"
#include "modes/colorwave.h"
#include "modes/dashboard.h"
#include "modes/dotter.h"
#include "modes/fire.h"
#include "modes/gameOfLife.h"
#include "modes/snake.h"

#include "shared/network/WiFiLoginManager.h"
#include "shared/persistence/persistenceManager.h"
#include "shared/serialWrapper.h"
#include <Arduino.h>

// manages all modes, they shouldn't be accessed directly!
namespace Controller {
enum Mode : uint8_t { OFF = 0, STATIC, LOGIN, CLOCK, DASHBOARD, COLORWAVE, FIRE, DOTTER, GOL, SNAKE, SIZE };

void turnOff();
void setMode(uint8_t new_mode);
void printText(const String& msg);
void showLogin();

namespace {
Mode mode;
uint32_t timeout = 0;

// TODO struct ModeInterface (Mode, name, setup, loop, select, deselect)

void _toMode(Mode new_mode) {
    if (new_mode == mode) return;
    print(F("New Mode: "));
    switch (new_mode) {
    case OFF:
        println(F("OFF"));
        turnOff();
        break;
    case STATIC:
        println(F("STATIC"));
        Display::clear();
        break;
    case LOGIN:
        println(F("LOGIN"));
        showLogin();
        break;
    case CLOCK:
        println(F("CLOCK"));
        Modes_Clock::updateScreen();
        break;
    case DASHBOARD:
        println(F("DASHBOARD"));
        Modes_Dashboard::updateScreen();
        break;
    case COLORWAVE:
        println(F("COLORWAVE"));
        Modes_Colorwave::updateScreen();
        break;
    case FIRE:
        println(F("FIRE"));
        Modes_Fire::updateScreen();
        break;
    case DOTTER:
        println(F("DOTTER"));
        Display::clear();
        break;
    case GOL:
        println(F("GOL"));
        Modes_GOL::reset();
        break;
    case SNAKE:
        println(F("SNAKE"));
        Modes_Snake::reset();
        break;
    default:
        println(F("UNKNOWN, discarding..."));
        _toMode(OFF);
        return;
    }
    mode = new_mode;
}
} // namespace

void hideAfter(uint32_t milliseconds) { timeout = millis() + milliseconds; }

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
    Display::setupText();
    Display::printText(msg);
    setMode(STATIC);
}

void showLogin() {
    String msg = WiFiLoginManager::getHostname();
    msg.reserve(80);
    msg += F("\n");
    msg += WiFi.localIP().toString();
    Display::clear();
    Display::setupText();
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

    // only to prevent log spam, PM deduplicates too
    if (new_mode == mode) return;

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
    Modes_Dashboard::setup();
    Modes_Colorwave::setup();
    Modes_Fire::setup();
    Modes_Dotter::setup();
    Modes_GOL::setup();
    Modes_Snake::setup();
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
    case DASHBOARD:
        Modes_Dashboard::loop();
        break;
    case COLORWAVE:
        Modes_Colorwave::loop();
        break;
    case FIRE:
        Modes_Fire::loop();
        break;
    case DOTTER:
        Modes_Dotter::loop();
        break;
    case GOL:
        Modes_GOL::loop();
        break;
    case SNAKE:
        Modes_Snake::loop();
        break;
    }
}

} // namespace Controller
