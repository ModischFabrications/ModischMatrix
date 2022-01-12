#pragma once

#include "display.h"
#include "shared/serialWrapper.h"
#include "time.h"
#include <Arduino.h>

// maybe morph using https://github.com/hwiguna/HariFun_166_Morphing_Clock/blob/master/Latest/MorphingClock/Digit.cpp

namespace Modes_Clock {
namespace {
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

const uint16_t UPDATE_DELAY = 30 * 1000;

struct tm timeinfo;
void updateLocalTime() {
    if (!getLocalTime(&timeinfo)) {
        logError(F("Failed to obtain time"));
    }
}

char out[10];
uint32_t nextUpdate = 0;
void displayLocalTime() {
    uint32_t now = millis();
    if (now < nextUpdate) return;
    nextUpdate = now + UPDATE_DELAY;
    strftime(out, sizeof(out), "%H:%M", &timeinfo);
    Display::screen->setTextSize(2);
    Display::screen->setCursor(3, 8);
    Display::screen->clearScreen();
    Display::screen->print(out);
    print(F("C:"));
    printlnRaw(out);
}
} // namespace

void setup() { configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); }

void loop() {
    updateLocalTime();
    displayLocalTime();
    delay(100);
}

} // namespace Modes_Clock