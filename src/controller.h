#pragma once

#include "display.h"

#include "modes/clock.h"
#include "modes/colorwave.h"
#include "modes/dashboard.h"
#include "modes/fire.h"
#include "modes/gameOfLife.h"
#include "modes/snake.h"
#include "modes/sprinkles.h"
#include "modes/freedraw.h"

#include "shared/network/WiFiLoginManager.h"
#include "shared/persistence/persistenceManager.h"
#include "shared/serialWrapper.h"
#include <Arduino.h>

// manages all modes, they shouldn't be accessed directly!
namespace Controller {
enum Mode : uint8_t { OFF = 0, STATIC, LOGIN, CLOCK, DASHBOARD, COLORWAVE, FIRE, SPRINKLES, GOL, SNAKE, FREEDRAW, SIZE };

typedef void (*cMethod)();
struct ModeInterface {
    const Mode mode;
    const __FlashStringHelper* name;
    const cMethod setup;
    const cMethod loop;
    const cMethod select;
    // const cMethod deSelect;
};

void turnOff();
void setMode(uint8_t new_mode);
void printText(const String& msg);
void showLogin();

namespace {
// shall have equal index to Mode enum
// this would be better suited for a map but arduino C++ doesn't have one
const ModeInterface modes[Mode::SIZE] = {
    {OFF, F("OFF"), nullptr, nullptr, turnOff},
    {STATIC, F("STATIC"), nullptr, nullptr, Display::clear},
    {LOGIN, F("LOGIN"), nullptr, nullptr, showLogin},
    {CLOCK, F("CLOCK"), Modes_Clock::setup, Modes_Clock::loop, Modes_Clock::updateScreen},
    {DASHBOARD, F("DASHBOARD"), Modes_Dashboard::setup, Modes_Dashboard::loop, Modes_Dashboard::updateScreen},
    {COLORWAVE, F("COLORWAVE"), Modes_Colorwave::setup, Modes_Colorwave::loop, Modes_Colorwave::updateScreen},
    {FIRE, F("FIRE"), Modes_Fire::setup, Modes_Fire::loop, Modes_Fire::reset},
    {SPRINKLES, F("SPRINKLES"), Modes_Sprinkles::setup, Modes_Sprinkles::loop, Modes_Sprinkles::reset},
    {GOL, F("GOL"), Modes_GOL::setup, Modes_GOL::loop, Modes_GOL::reset},
    {SNAKE, F("SNAKE"), Modes_Snake::setup, Modes_Snake::loop, Modes_Snake::reset},
    {FREEDRAW, F("FREEDRAW"), Modes_FreeDraw::setup, Modes_FreeDraw::loop, Modes_FreeDraw::reset},
};

const ModeInterface* activeMode = &modes[0];
uint32_t timeout = 0;

void _toMode(Mode new_mode) {
    if (new_mode == activeMode->mode) return;
    print(F("New Mode: "));

    for (const ModeInterface& m : modes) {
        if (m.mode == new_mode) {
            println(modes[new_mode].name);
            activeMode = &m;
            activeMode->select();
            return;
        }
    }

    println(F("UNKNOWN, discarding..."));
    _toMode(OFF);
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
    if (new_mode == activeMode->mode) return;

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

    for (const ModeInterface& m : modes) {
        if (m.setup == nullptr) continue;
        m.setup();
    }
}

void loop() {
    uint32_t now = millis();
    if (timeout != 0 && now > timeout) {
        println(F("C: Timeout"));
        timeout = 0;
        setMode(OFF);
    }

    if (activeMode->loop != nullptr) activeMode->loop();
}

} // namespace Controller
