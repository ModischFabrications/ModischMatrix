#pragma once

#include "controller.h"
#include "pinout.h"
#include "shared/rebootManager.h"
#include "shared/serialWrapper.h"
#include <Arduino.h>

namespace Input_Touch {

// they seem to be 70 when unconnected and 13 when touched on this board
const uint16_t TOUCH_THRESHOLD = 20; // triggered *below* this!

enum Touched : uint8_t { NONE, TOP, LEFT, RIGHT };

Touched touched = NONE;

void touchedTop() { touched = TOP; }
void touchedLeft() { touched = LEFT; }
void touchedRight() { touched = RIGHT; }

// touchRead takes ~ 0.5ms
void logTouch(uint8_t pin) {
    uint16_t touchVal = touchRead(pin);
    print(F("Touch P"));
    printRaw(pin);
    print(F(" value: "));
    printlnRaw(touchVal);
}

void logAllTouches() {
    for (uint8_t p : Pinout::TOUCH) {
        logTouch(p);
    }
}

void setup() {
    logAllTouches();
    delay(10); // make sure touch pins are loaded
    // touchAttachInterrupt(Pinout::TOUCH_TOP, touchedTop, TOUCH_THRESHOLD);
    touchAttachInterrupt(Pinout::TOUCH_LEFT, touchedLeft, TOUCH_THRESHOLD);
    touchAttachInterrupt(Pinout::TOUCH_RIGHT, touchedRight, TOUCH_THRESHOLD);
}

void loop() {
    switch (touched) {
    case NONE:
    default:
        return;
    case TOP:
        Controller::turnOff();
        break;
    case LEFT:
        Controller::printText(F("Ooooouuh \n left is \n love!"));
        Controller::hideAfter(2000);
        break;
    case RIGHT:
        Controller::printText(F("Ooooouuh \n right is \n ripe!"));
        Controller::hideAfter(2000);
        break;
    }
    logAllTouches();
    touched = NONE;
    RebootManager::setActive();
}

} // namespace Input_Touch
