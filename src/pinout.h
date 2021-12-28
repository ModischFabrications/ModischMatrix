#pragma once

#include <Arduino.h>

namespace Pinout {

// https://i0.wp.com/randomnerdtutorials.com/wp-content/uploads/2018/08/ESP32-DOIT-DEVKIT-V1-Board-Pinout-30-GPIOs-Copy.png

// ESP32 buildin
const uint8_t TOUCH_LEFT = 2; // able to interrupt
const uint8_t TOUCH_TOP = 3;  // able to interrupt
const uint8_t TOUCH_RIGHT = 4;
const uint8_t TOUCH[] = {TOUCH_LEFT, TOUCH_TOP, TOUCH_RIGHT};

const uint8_t STATUS_LED = LED_BUILTIN;

/* ----------------------------*/
// -- HUB75E pinout
// R1 | G1
// B1 | GND
// R2 | G2
// B2 | E
//  A | B
//  C | D
// CLK| LAT
// OE | GND

/* ----------------------------*/
// -- left side
const uint8_t R1 = 32;
const uint8_t BL1 = 33;
const uint8_t R2 = 25;
const uint8_t BL2 = 26;
const uint8_t CH_A = 27;
const uint8_t CH_C = 14;
const uint8_t CLK = 12;
const uint8_t OE = 13;
// -- right side
const uint8_t G1 = 23;
// GND
const uint8_t G2 = 22;
const uint8_t CH_E = 21;
const uint8_t CH_B = 19;
const uint8_t CH_D = 18;
const uint8_t LAT = 5;
// GND
/* ----------------------------*/

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    // Matrix sets up itself
}

} // namespace Pinout
