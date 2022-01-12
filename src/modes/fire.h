#pragma once

#include "display.h"
#include "shared/serialWrapper.h"
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <FastLED.h>

namespace Modes_Fire {
namespace {

const uint8_t WIDTH = Display::PANEL_RES_X;
const uint8_t HEIGHT = Display::PANEL_RES_Y;

// heavily inspired by
// https://www.reddit.com/r/FastLED/comments/lbnp53/fake_fire_w_fastled_a_project_ive_been_toying/
// https://pastebin.com/pxMLWP7j
// Sadly, neither pretty for fully adapted for this lib

// Color Palette(s)
DEFINE_GRADIENT_PALETTE(fire_pal){
    0,   0,   0,   0, // black
    80,  255, 0,   0, // red
    230, 255, 255, 0, // bright yellow
    255, 255, 255, 40 // full white
};

DEFINE_GRADIENT_PALETTE(ice_pal){
    0,   0,  0,   0,   // black
    80,  0,  0,   255, // Dark Blue
    230, 0,  255, 255, // bright Blueish
    255, 20, 255, 255  // full white
};

DEFINE_GRADIENT_PALETTE(emrld_pal){
    0,   0,   0,   0,   // black
    80,  0,   255, 0,   // Dark green
    230, 100, 255, 100, // bright Greenish
    255, 50,  255, 50   // full white
};

const CRGBPalette256 activePalette = fire_pal; // select the pallet to use

// Turn coordinates X/Y into string of LEDs
uint16_t XY(uint8_t x, uint8_t y) {
    uint16_t i;
    i = (y * HEIGHT) + x;

    return i;
}

uint8_t intensity[WIDTH * HEIGHT];
void updateScreen() {
    // update intensity with neighboring average of lower row
    for (uint8_t row = (HEIGHT - 1); row >= 1; row--) {
        for (uint8_t col = (WIDTH - 2); col >= 1; col--) {
            uint16_t avgCol =
                (intensity[XY(row - 1, col - 1)] + intensity[XY(row - 1, col)] + intensity[XY(row - 1, col + 1)]) / 3;
            intensity[XY(row, col)] = avgCol / random8(1, 3); // magic numbers, smaller seems to make bigger flames
        }
    }

    // Create new fire column
    // If one of the LEDs on either side is already 'on fire' then there is a higher chance of ignition
    // If not, it has a low chance to self-ignite
    byte catchOn = 50;    // Chance to spread fire - higher number less chance (0-254) default: 50
    byte ignite = 253;    // Chance to self-ignite - higher number less chance (0-254) default: 253
    byte chgchance = 125; // higher number less chance (0-254) default: 245

    for (int i = 0; i <= (WIDTH - 1); i++) {
        if (random8() > chgchance)
            continue;
        byte rCol = random8();
        if (i >= 1 && i <= (WIDTH - 2)) {
            if (intensity[XY(0, i - 1)] > 0 || intensity[XY(0, i + 1)] > 0) {
                if (rCol > catchOn) {
                    rCol = random8(250, 255);
                } else {
                    rCol = 0;
                }
            } else {
                if (rCol > ignite) {
                    rCol = 255;
                } else {
                    rCol = 0;
                }
            }
        } else {
            if (rCol > ignite) {
                rCol = 255;
            } else {
                rCol = 0;
            }
        }
        intensity[XY(0, i)] = rCol;
    }

    // apply the colors to the CRGB array
    for (int i = 0; i <= ((WIDTH * HEIGHT) - 1); i++) {
        CRGB c = ColorFromPalette(activePalette, intensity[i]);
        uint8_t x = i / HEIGHT;
        uint8_t y = i % HEIGHT;
        if (true)
            y = HEIGHT - 1 - y;
        Display::screen->drawPixelRGB888(x, y, c.r, c.g, c.b);
    }
}

} // namespace

void setup() {}

void loop() {
    updateScreen();
    delay(60);
}

} // namespace Modes_Fire