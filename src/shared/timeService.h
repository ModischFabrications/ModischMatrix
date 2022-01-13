#pragma once

#include "shared/serialWrapper.h"
#include <Arduino.h>
#include <time.h>

namespace TimeService {
// see https://www.cplusplus.com/reference/ctime/strftime/ for formatting hints
struct tm timeinfo;

namespace {
const uint16_t UPDATE_DELAY = 30 * 1000;

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

void updateLocalTime() {
    if (!getLocalTime(&timeinfo)) {
        logError(F("Failed to obtain time"));
    }
}
} // namespace

void setup() { configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); }

uint32_t nextUpdate = 0;
void loop() {
    uint32_t now = millis();
    if (now < nextUpdate)
        return;
    nextUpdate = now + UPDATE_DELAY;
    updateLocalTime();
}

} // namespace TimeService