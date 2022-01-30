#pragma once

#include "pinout.h"
#include "shared/serialWrapper.h"
#include "shared/simpleTimer.h"
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h> // https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA

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

SimpleTimer timer;

void clear() { screen->clearScreen(); }

String reformatText(const String& msg) {
    String val = msg;
    val.replace(F("\\n"), F("\n"));

    val.replace(F("ä"), F("\x84"));
    val.replace(F("ö"), F("\x94"));
    val.replace(F("ü"), F("\x81"));

    val.replace(F("\\heart"), F("\x03"));
    val.replace(F("❤"), F("\x03"));
    val.replace(F("♥"), F("\x03"));

    val.replace(F("->"), F("\x1A"));
    val.replace(F("<-"), F("\x1B"));

    val.replace(F("%"), F("\x25"));
    val.replace(F("+"), F("\x2B"));
    val.replace(F("€"), F("\x9B"));

    return val;
}

void printText(const String& msg, uint16_t colorText = white, uint16_t colorBg = black) {
    screen->clearScreen();
    screen->setTextColor(colorText, colorBg);
    screen->setCursor(0, 0);
    screen->print(reformatText(msg));
}

void setBrightness(uint8_t brightness) {
    screen->setBrightness8(brightness); // 0-255
}

void setupText() {
    screen->cp437(true);
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
    timer.registerCall([]() { colorDot(black); }, 100);
}

void flashBorder() {
    screen->drawRect(0, 0, screen->width(), screen->height(), grey);
    timer.registerCall([]() { screen->drawRect(0, 0, screen->width(), screen->height(), black); }, 100);
}

void flashScreen() {
    screen->clearScreen();
    screen->fillScreen(grey);
    timer.registerCall([]() { screen->clearScreen(); }, 100);
}

void setup() {
    println(F("Starting matrix..."));

    setupPanel();
}

void loop() { timer.loop(); }

} // namespace Display
