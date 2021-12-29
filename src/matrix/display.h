#pragma once

#include "pinout.h"
#include "shared/serialWrapper.h"
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h> // https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA

namespace Display {
using namespace Pinout;

const uint8_t PANEL_RES_X = 64;
const uint8_t PANEL_RES_Y = 32;
const uint8_t PANEL_CHAIN = 1;

MatrixPanel_I2S_DMA* screen = nullptr;

const uint16_t black = screen->color565(0, 0, 0);
const uint16_t white = screen->color565(255, 255, 255);
const uint16_t grey = screen->color565(64, 64, 64);

void clear() { screen->clearScreen(); }

void printText(const String& msg, uint16_t colorText = white, uint16_t colorBg = black) {
    screen->clearScreen();
    screen->setTextColor(colorText, colorBg);
    screen->setCursor(2, 2);
    screen->print(msg);
}

void setBrightness(uint8_t brightness) {
    screen->setBrightness8(90); // 0-255
}

void setupText() {
    screen->setTextSize(1); // size 1 == 8 pixels high
    screen->setTextWrap(true);
}

void setupPanel() {
    HUB75_I2S_CFG::i2s_pins _pins = {R1,   G1,   BL1,  R2,   G2,  BL2, CH_A,
                                     CH_B, CH_C, CH_D, CH_E, LAT, OE,  CLK};
    // Module configuration
    HUB75_I2S_CFG mxconfig(PANEL_RES_X, // module width
                           PANEL_RES_Y, // module height
                           PANEL_CHAIN, // Chain length
                           _pins);

    mxconfig.gpio.e = CH_E;
    mxconfig.clkphase = false;
    mxconfig.driver = HUB75_I2S_CFG::FM6126A;

    screen = new MatrixPanel_I2S_DMA(mxconfig);
    screen->begin();

    setupText();

    // initial values come from persistence/controller
}

void flashBorder() {
    screen->drawRect(0, 0, screen->width(), screen->height(), grey);
    delay(200);
    screen->drawRect(0, 0, screen->width(), screen->height(), black);
}

void flashScreen() {
    screen->clearScreen();
    screen->fillScreenRGB888(127, 127, 127);
    delay(1000);
    screen->clearScreen();
}

// TODO copy from Tests\animations.h

void setup() {
    println(F("Starting matrix..."));

    setupPanel();

    flashScreen();
}

} // namespace Display
