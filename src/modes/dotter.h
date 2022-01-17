#pragma once

#include "display.h"
#include "shared/serialWrapper.h"
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// dots with random color and random positions. Starting bright, fading to black.
// more examples: https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA/tree/master/examples/AuroraDemo

namespace Modes_Dotter {
namespace {

const uint16_t DOT_DELAY = 100;
const uint8_t WIDTH = Display::PANEL_RES_X;
const uint8_t HEIGHT = Display::PANEL_RES_Y;

void newDot() {
    Display::screen->drawPixelRGB888(random8(0, WIDTH), random8(0, HEIGHT), random8(), random8(), random8());
}

void updateScreen() {
    // TODO fade to black; no (efficient) idea without FastLED
}

} // namespace

void setup() {}

uint32_t nextDot = 0;
void loop() {
    updateScreen();
    delay(10);
    uint32_t now = millis();
    if (now < nextDot) return;
    nextDot = now + DOT_DELAY;
    newDot();
}

} // namespace Modes_Dotter