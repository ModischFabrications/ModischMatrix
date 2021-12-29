#pragma once

#include "matrix/display.h"
#include "shared/persistence/persistenceManager.h"
#include <Arduino.h>

namespace Controller {
bool visible = true;

void setVisibility(bool on) {
    visible = on;
    Display::flashBorder();
    Display::clear();      
}

void setMode(uint8_t mode) {
    // TODO update display
    Config::Configuration config = PersistenceManager::get();
    config.mode = mode;
    PersistenceManager::set(config);
}

void setBrightness(uint8_t brightness) {
    // TODO update display
    Config::Configuration config = PersistenceManager::get();
    config.brightness = brightness;
    PersistenceManager::set(config);
}

void print(const String& msg) {
    if (!visible)
        return;
    Display::printText(msg);
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

} // namespace Controller
