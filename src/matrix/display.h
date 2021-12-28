#pragma once

#include "pinout.h"
#include "shared/serialWrapper.h"
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h> // https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA

namespace Matrix {
using namespace Pinout;

const uint8_t PANEL_RES_X = 64;
const uint8_t PANEL_RES_Y = 32;
const uint8_t PANEL_CHAIN = 1;

MatrixPanel_I2S_DMA* dma_display = nullptr;

void set_brightness(uint8_t brightness) {
    dma_display->setBrightness8(90); // 0-255
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

    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    dma_display->begin();

    // TODO load from persistence
    set_brightness(90);
}

void flashScreen() {
    dma_display->clearScreen();
    dma_display->fillScreenRGB888(127, 127, 127);
    delay(200);
}

// TODO copy from Tests\animations.h

void setup() {
    println(F("Starting matrix..."));

    setupPanel();

    flashScreen();
}

} // namespace Matrix
