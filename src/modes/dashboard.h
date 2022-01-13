#pragma once

#include "display.h"
#include "shared/serialWrapper.h"
#include "shared/timeService.h"
#include <Arduino.h>

// maybe morph using https://github.com/hwiguna/HariFun_166_Morphing_Clock/blob/master/Latest/MorphingClock/Digit.cpp

namespace Modes_Dashboard {
namespace {
const uint16_t UPDATE_DELAY = 30 * 1000;

char out[16];
void updateScreen() {
    MatrixPanel_I2S_DMA* s = Display::screen;
    s->clearScreen();
    // TODO draw all static background elements as icon

    s->setTextSize(1);

    s->setCursor(4, 4);
    strftime(out, sizeof(out), "%H:%M", &TimeService::timeinfo);
    s->print(out);

    s->setCursor(2, 24);
    strftime(out, sizeof(out), "%d.%m.%Y", &TimeService::timeinfo);
    s->print(out);

    // TODO weather

    print(F("."));
}
} // namespace

void setup() {}

uint32_t nextUpdate = 0;
void loop() {
    uint32_t now = millis();
    if (now < nextUpdate)
        return;
    nextUpdate = now + UPDATE_DELAY;
    updateScreen();
}

} // namespace Modes_Dashboard