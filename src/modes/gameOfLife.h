#pragma once

#include "display.h"
#include "shared/serialWrapper.h"
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <FastLED.h>

// realtime version of game of life. Random seed, running forever.
// heavily inspired by
// ....?

namespace Modes_GOL {
namespace {

const uint16_t UPDATE_DELAY = 500;
const uint8_t WIDTH = Display::PANEL_RES_X;
const uint8_t HEIGHT = Display::PANEL_RES_Y;

void move() {}

void draw() {}

void updateScreen() {
    move();
    draw();
}

} // namespace

void reset() { println(F("GOL: Clearing board, creating new seed")); }

void setup() {}

uint32_t nextUpdate = 0;
void loop() {
    uint32_t now = millis();
    if (now < nextUpdate) return;
    nextUpdate = now + UPDATE_DELAY;
    updateScreen();
}

} // namespace Modes_GOL