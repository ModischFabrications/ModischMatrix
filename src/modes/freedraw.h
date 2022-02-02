#pragma once

#include "display.h"
#include "shared/serialWrapper.h"
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// fully async

namespace Modes_FreeDraw {
namespace {} // namespace

void pixel(uint8_t x, uint8_t y, const String& color) { Display::screen->drawPixel(x, y, Display::parseRGB(color)); }

void fill(const String& color) { Display::screen->fillScreen(Display::parseRGB(color)); }

void clear() { Display::clear(); }

void reset() { Display::clear(); }

void setup() {}

void loop() {
    // nothing to do, but this should ease async load
    delay(5);
}

} // namespace Modes_FreeDraw