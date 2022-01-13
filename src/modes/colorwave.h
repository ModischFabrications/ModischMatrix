#pragma once

#include "display.h"
#include "shared/serialWrapper.h"
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <FastLED.h>

namespace Modes_Colorwave {
namespace {

const uint8_t WIDTH = Display::PANEL_RES_X;
const uint8_t HEIGHT = Display::PANEL_RES_Y;

// heavily inspired by
// https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA/blob/master/examples/2_PatternPlasma/2_PatternPlasma.ino

uint16_t time_counter = 0, cycles = 0;
CRGB currentColor;
CRGBPalette16 palettes[] = {HeatColors_p, LavaColors_p, RainbowColors_p, RainbowStripeColors_p, CloudColors_p};
CRGBPalette16 currentPalette = palettes[0];

void updateScreen() {
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            int16_t v = 0;
            uint8_t wibble = sin8(time_counter);
            v += sin16(x * wibble * 3 + time_counter);
            v += cos16(y * (128 - wibble) + time_counter);
            v += sin16(y * x * cos8(-time_counter) / 8);

            currentColor = ColorFromPalette(currentPalette, (v >> 8) + 127);
            Display::screen->drawPixelRGB888(x, y, currentColor.r, currentColor.g, currentColor.b);
        }
    }

    ++time_counter;
    ++cycles;

    if (cycles >= 1024) {
        time_counter = 0;
        cycles = 0;
        currentPalette = palettes[random(0, sizeof(palettes) / sizeof(palettes[0]))];
    }
}

} // namespace

void setup() { currentPalette = RainbowColors_p; }

void loop() { updateScreen(); }

} // namespace Modes_Colorwave