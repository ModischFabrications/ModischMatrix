#pragma once

#include "display.h"
#include "shared/serialWrapper.h"
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

// multiplayer snake. Eat snacks to grow, let others crash into you.

// TODO const and pointer everything!

namespace Modes_Snake {

enum Direction : uint8_t { OFF, UP, DOWN, LEFT, RIGHT, DEAD };

namespace {
const uint16_t UPDATE_DELAY = 500;
const uint16_t SNACK_DELAY = 10 * 1000;

const uint8_t N_MAX_PLAYERS = 4;
const uint8_t N_MAX_SNACKS = 4;
// trailing to darkness
const uint16_t C_PLAYERS[] = {0x001F, 0x07FF, 0xF81F, 0xFFE0};
const uint16_t C_FOOD = 0xFFFF;
const uint8_t MIN_LENGTH = 4;
const uint8_t MAX_LENGTH = 32;

const uint8_t WIDTH = Display::PANEL_RES_X;
const uint8_t HEIGHT = Display::PANEL_RES_Y;

struct XY {
    int8_t x = 0;
    int8_t y = 0;

    XY() : x{0}, y{0} {}
    XY(int8_t _x, int8_t _y) : x{_x}, y{_y} {}
    bool const operator==(const XY other) const { return x == other.x && y == other.y; }
    XY const operator+(const XY other) const { return XY(x + other.x, y + other.y); }
    XY const operator+=(const XY other) {
        x += other.x;
        y += other.y;

        while (x >= WIDTH)
            x -= WIDTH;
        while (y >= HEIGHT)
            y -= HEIGHT;
        while (x < 0)
            x += WIDTH;
        while (y < 0)
            y += HEIGHT;

        this->x = x;
        this->y = y;

        return *this;
    }
};

struct Snake {
    Direction dir;
    XY pos[MAX_LENGTH];
    uint8_t len = MIN_LENGTH;
};

Snake players[N_MAX_PLAYERS];
XY snacks[N_MAX_SNACKS];
uint8_t iSnack = 0;

XY dirToPos(const Direction& dir) {
    switch (dir) {
    case OFF:
        return XY(0, 0);
    case UP:
        return XY(0, -1);
    case DOWN:
        return XY(0, 1);
    case LEFT:
        return XY(-1, 0);
    case RIGHT:
        return XY(1, 0);
    default:
        logError(F("invalid direction!"));
        return XY(0, 0);
    }
}

void printPlayer(uint8_t i) {
    print(F("P"));
    printRaw(i);
    print(F(": "));
    print(F("Dir "));
    printRaw((uint8_t)players[i].dir);
    print(F(", Pos "));
    for (uint8_t i = 0; i < players[i].len; i++) {
        XY pos = players[i].pos[i];
        printRaw((int16_t)pos.x);
        print(F(","));
        printRaw((int16_t)pos.y);
        print(F("|"));
    }
    print(F(" -> Len "));
    printRaw((uint8_t)players[i].len);
    println();
}

void move() {
    for (Snake& sn : players) {
        if (sn.dir == DEAD || sn.dir == OFF) continue;
        for (uint8_t i = 1; i < sn.len; i++) {
            sn.pos[i] = sn.pos[i - 1];
        }
        sn.pos[0] = sn.pos[1] + dirToPos(sn.dir);
        // TODO check for collisions
    }
}

void draw() {
    Display::clear();
    for (uint8_t i = 0; i < N_MAX_PLAYERS; i++) {
        Snake& sn = players[i];
        if (sn.dir == DEAD) continue;
        Display::screen->drawPixel(sn.pos[0].x, sn.pos[0].y, C_PLAYERS[i]);
        for (uint8_t j = 1; j < sn.len; j++) {
            // TODO dim down by i*(256/MAX_LENGTH-1)
            Display::screen->drawPixel(sn.pos[j].x, sn.pos[j].y, C_PLAYERS[i]);
        }
        printPlayer(i);
    }
}

uint32_t nextSnack = 0;
void updateScreen() {
    move();
    draw();

    uint32_t now = millis();
    if (now < nextSnack) return;
    nextSnack = now + SNACK_DELAY;
    // TODO create snack instead
    println(F("New snack at [TODO]"));
    for (Snake& sn : players) {
        //sn.len += 1;
    }
}

} // namespace

void setDirection(uint8_t player, const Direction direction) {
    if (direction == OFF || direction == DEAD) return;
    if (players[player].dir == direction) return;
    players[player].dir = direction;
    print(F("P "));
    printRaw(player);
    print(F(" : DIR "));
    printlnRaw((uint8_t)direction);
}

void reset() {
    println(F("SN: Clearing board, creating new snakes"));
    Display::clear();
    for (Snake& sn : players) {
        sn.dir = OFF;
        sn.len = 4;
        XY pos = XY(random8(0, WIDTH), random8(0, HEIGHT));
        for (uint8_t i = 0; i < sn.len; i++) {
            sn.pos[i] = pos;
        }
    }
    printPlayer(0);
}

void setup() { reset(); }

uint32_t nextUpdate = 0;
void loop() {
    uint32_t now = millis();
    if (now < nextUpdate) return;
    nextUpdate = now + UPDATE_DELAY;
    updateScreen();
}

} // namespace Modes_Snake