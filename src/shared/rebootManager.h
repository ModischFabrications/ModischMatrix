#pragma once

#include "shared/serialWrapper.h"
#include <Arduino.h>

namespace RebootManager {

namespace {
// <49 days to prevent millis overflow
const uint32_t MIN_REBOOT_DELAY_MS = 24 * 60 * 60 * 1000;
const uint32_t INACTIVITY_THRESHOLD_MS = 30 * 60 * 1000;

uint32_t scheduledRestart = 0;

void rebootNow() {
    logWarning(F("Restarting in 5s..."));
    delay(5 * 1000);
    // TODO set flags to differentiate restart reason
    ESP.restart();
}

} // namespace

void setActive() { scheduledRestart = millis() + INACTIVITY_THRESHOLD_MS; }

void loop() {
    if (millis() > MIN_REBOOT_DELAY_MS && scheduledRestart < millis())
        rebootNow();
}

} // namespace RebootManager
