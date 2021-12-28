#pragma once

#include "shared/persistence/persistenceManager.h"
#include "matrix/display.h"
#include <Arduino.h>

namespace Controller {
void setVisibility(bool on) {
    // TODO ac
    Display::flashScreen();
}

void setMode(uint8_t mode) {
    // TODO update display
    PersistenceManager::configuration.mode = mode;
}

void updateConfig(){
    Config::Configuration config = PersistenceManager::get();
    Display::dma_display->setBrightness8(config.brightness);
}

// TODO ...

void setup() {
    setVisibility(true);
    // TODO
    PersistenceManager::registerListener(updateConfig);
}

} // namespace Controller
