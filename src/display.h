#pragma once

#define FASTLED_INTERNAL // disable pragma message

#include "pinout.h"
#include "shared/serialWrapper.h"
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h> // https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA
#include <FastLED.h>

// manages screen initialization and some utility methods. Bypassable by more complex interactions.

namespace Display {
using namespace Pinout;

const uint8_t PANEL_RES_X = 64;
const uint8_t PANEL_RES_Y = 32;
const uint8_t PANEL_CHAIN = 1;

MatrixPanel_I2S_DMA* screen = nullptr;

const uint16_t black = screen->color565(0, 0, 0);
const uint16_t white = screen->color565(255, 255, 255);
const uint16_t grey = screen->color565(64, 64, 64);
const uint16_t blue = screen->color565(3, 144, 252);
const uint16_t orange = screen->color565(252, 152, 3);

void clear() { screen->clearScreen(); }

void printText(const String& msg, uint16_t colorText = white, uint16_t colorBg = black) {
    screen->clearScreen();
    screen->setTextColor(colorText, colorBg);
    screen->setCursor(1, 1);
    screen->print(msg);
}

void setBrightness(uint8_t brightness) {
    screen->setBrightness8(brightness); // 0-255
}

void setupText() {
    screen->setTextSize(1); // size 1..3; 1 is 8 pixels high
    screen->setTextWrap(true);
}

void setupPanel() {
    HUB75_I2S_CFG::i2s_pins _pins = {R1, G1, BL1, R2, G2, BL2, CH_A, CH_B, CH_C, CH_D, CH_E, LAT, OE, CLK};
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

void colorDot(uint16_t color) {
    // top right corner
    screen->drawPixel(screen->width() - 1, 0, color);
}

void flashDot() {
    colorDot(white);
    delay(200);
    colorDot(black);
}

void flashBorder() {
    screen->drawRect(0, 0, screen->width(), screen->height(), grey);
    delay(200);
    screen->drawRect(0, 0, screen->width(), screen->height(), black);
}

void flashScreen() {
    screen->clearScreen();
    screen->fillScreen(grey);
    delay(1000);
    screen->clearScreen();
}

void setup() {
    println(F("Starting matrix..."));

    setupPanel();

    flashScreen();
}

} // namespace Display
