#pragma once

#include "display.h"
#include "shared/serialWrapper.h"
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <FastLED.h>

// realtime version of conways Game of Life. Random seed, running forever.
// https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
// heavily inspired by https://codereview.stackexchange.com/questions/47167/conways-game-of-life-in-c/47179#47179

// a lot of pointer witchcraft for performance, see https://stackoverflow.com/a/33983464/7924138

namespace Modes_GOL {
namespace {

const uint16_t UPDATE_DELAY = 500;
const uint8_t ALIVE_CHANCE = 45; // of 255

const uint8_t WIDTH = Display::PANEL_RES_X;
const uint8_t HEIGHT = Display::PANEL_RES_Y;

const uint16_t C_ALIVE = Display::screen->color565(115, 127, 115);
const uint16_t C_DEAD = Display::screen->color565(6, 2, 2);

bool storeA[WIDTH][HEIGHT];
bool storeB[WIDTH][HEIGHT];

auto prevState = &storeA;
auto currState = &storeB;

// TODO could theoretically be passed as a parameter
void reseed() {
    for (uint8_t y = 4; y < HEIGHT - 4; y++) {
        for (uint8_t x = 4; x < WIDTH - 4; x++) {
            (*currState)[x][y] = random8() <= ALIVE_CHANCE;
            //print((*currState)[x][y] ? F("X") : F(" "));
        }
        //println();
    }
}

uint8_t nNeighbors(uint8_t x_pos, uint8_t y_pos, bool array[WIDTH][HEIGHT]) {
    uint8_t count = 0;
    // prevent overflow access due to matrix bounds
    for (uint8_t y = max(y_pos - 1, 0); y <= min(y_pos + 1, HEIGHT - 1); y++) {
        for (uint8_t x = max(x_pos - 1, 0); x <= min(x_pos + 1, WIDTH - 1); x++) {
            if (x_pos == x && y_pos == y) continue;
            if (array[x][y]) count++;
        }
    }

    return count;
}

// TODO pass rules as parameters
// default rule: B3/S23 -> born with 3 neighbors, survives with 2 or 3, dies otherwise
bool isAlive(bool alive, uint8_t neighbors) {
    if (!alive && neighbors == 3) return true;
    if (alive && (neighbors == 2 || neighbors == 3)) return true;
    return false;
}

// pointer witchcraft, took me a while
void swap(bool (**a)[WIDTH][HEIGHT], bool (**b)[WIDTH][HEIGHT]) {
    auto tmp = *a;
    *a = *b;
    *b = tmp;
}

void update() {
    // flipping pointers is cheaper than copying arrays
    swap(&prevState, &currState);

    for (uint8_t y = 0; y < HEIGHT; y++) {
        for (uint8_t x = 0; x < WIDTH; x++) {
            uint8_t neighbors = nNeighbors(x, y, *prevState);
            (*currState)[x][y] = isAlive((*prevState)[x][y], neighbors);
        }
    }
}

void draw() {
    // could probably be optimised to be a single GFX call
    for (uint8_t y = 0; y < HEIGHT; y++) {
        for (uint8_t x = 0; x < WIDTH; x++) {
            // TODO blend with previous value
            Display::screen->drawPixel(x, y, (*currState)[x][y] ? C_ALIVE : C_DEAD);
        }
    }
}

void updateScreen() {
    update();
    draw();
}

} // namespace

void reset() {
    println(F("GOL: Clearing board, creating new seed"));
    Display::clear();
    reseed();
}

void setup() {}

uint32_t nextUpdate = 0;
void loop() {
    uint32_t now = millis();
    if (now < nextUpdate) return;
    nextUpdate = now + UPDATE_DELAY;
    updateScreen();
}

} // namespace Modes_GOL