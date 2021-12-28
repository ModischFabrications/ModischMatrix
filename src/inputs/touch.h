#pragma once

#include "pinout.h"
#include "shared/serialWrapper.h"
#include <Arduino.h>

namespace Input_Touch {

const uint16_t TOUCH_THRESHOLD = 40;

// TODO attach callbacks

void setup() {
    // touchRead takes ~ 0.5ms
    for (uint8_t i_b : Pinout::TOUCH) {
        uint16_t touchVal = touchRead(i_b);
        print(F("Touch P"));
        printRaw(i_b);
        print(F(" initial: "));
        printlnRaw(touchVal);

        // set in next compile from prev prints
        touchAttachInterrupt(i_b, nullptr, TOUCH_THRESHOLD);
    }
}

} // namespace Input_Touch
