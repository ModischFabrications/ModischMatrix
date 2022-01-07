#pragma once

#include "display.h"
#include "shared/serialWrapper.h"
#include "time.h"
#include <Arduino.h>

// maybe morph using https://github.com/hwiguna/HariFun_166_Morphing_Clock/blob/master/Latest/MorphingClock/Digit.cpp

namespace Modes_Clock {

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

struct tm timeinfo;
void updateLocalTime() {
    if (!getLocalTime(&timeinfo)) {
        logError(F("Failed to obtain time"));
    }
}

void printLocalTime() { Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S"); }

char out[10];
void displayLocalTime() {
    strftime(out, sizeof(out), "%H:%M", &timeinfo);
    Display::screen->setTextSize(2);
    Display::screen->setCursor(2, 8);
    Display::screen->clearScreen();
    Display::screen->print(out);
}

void setup() {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    updateLocalTime();
    printLocalTime();
}

void loop() {
    updateLocalTime();
    displayLocalTime();
    delay(500);
}

} // namespace Modes_Clock