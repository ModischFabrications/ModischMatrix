#pragma once

#include "display.h"
#include "shared/serialWrapper.h"
#include <Arduino.h>

namespace Modes_Clock {

void setup() {
    // TODO setup NTP
}

void loop() {
    // TODO write NTP time to matrix
    Display::printText(F("04:20"));
    delay(500);
}

} // namespace Modes_Clock