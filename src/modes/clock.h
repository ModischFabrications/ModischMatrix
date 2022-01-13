#pragma once

#include "display.h"
#include "shared/serialWrapper.h"
#include "shared/timeService.h"
#include <Arduino.h>

// maybe morph using https://github.com/hwiguna/HariFun_166_Morphing_Clock/blob/master/Latest/MorphingClock/Digit.cpp

namespace Modes_Clock {
namespace {
const uint16_t UPDATE_DELAY = 30 * 1000;

char out[10];
void updateScreen() {
    strftime(out, sizeof(out), "%H:%M", &TimeService::timeinfo);
    Display::screen->setTextSize(2);
    Display::screen->setCursor(3, 8);
    Display::screen->clearScreen();
    Display::screen->print(out);
    print(F("."));
}
} // namespace

void setup() {}

uint32_t nextUpdate = 0;
void loop() {
    uint32_t now = millis();
    if (now < nextUpdate) return;
    nextUpdate = now + UPDATE_DELAY;
    updateScreen();
}

} // namespace Modes_Clock