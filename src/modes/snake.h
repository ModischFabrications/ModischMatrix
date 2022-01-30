#pragma once

#include "display.h"
#include "shared/serialWrapper.h"
#include <Arduino.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
#include <FastLED.h>

// multiplayer snake. Eat snacks to grow, let others crash into you.

namespace Modes_Snake {

enum Direction : uint8_t { OFF = 0, UP, DOWN, LEFT, RIGHT, DEAD };

namespace {
const uint16_t UPDATE_DELAY = 400; // 400*64 ~= 30s max
const uint16_t SNACK_DELAY = 10 * 1000;

const uint8_t N_MAX_PLAYERS = 4;
const uint8_t N_MAX_SNACKS = 6;

// copy from/to website css!
const CRGB C_PLAYERS[] = {
    CRGB(231, 76, 60),
    CRGB(46, 204, 113),
    CRGB(52, 152, 219),
    CRGB(185, 173, 0),
};
const CRGB C_SNACK = CRGB(200, 200, 200);
const CRGB C_DEAD = CRGB(40, 40, 40);
const uint8_t MIN_LENGTH = 4;
const uint8_t MAX_LENGTH = 32;

const uint8_t WIDTH = Display::PANEL_RES_X;
const uint8_t HEIGHT = Display::PANEL_RES_Y;

struct XY {
    int8_t x = 0;
    int8_t y = 0;

    XY() : x{0}, y{0} {}
    XY(int8_t _x, int8_t _y) : x{_x}, y{_y} {}
    bool const operator==(const XY& other) const { return x == other.x && y == other.y; }
    XY const operator+(const XY& other) const { return XY(x + other.x, y + other.y); }
    XY const operator+=(const XY& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    void wrap() {
        while (x >= WIDTH)
            x -= WIDTH;
        while (y >= HEIGHT)
            y -= HEIGHT;
        while (x < 0)
            x += WIDTH;
        while (y < 0)
            y += HEIGHT;
    }

    static XY random() { return XY(random8(0, WIDTH), random8(0, HEIGHT)); }
};

struct Snake {
    Direction dir;
    XY pos[MAX_LENGTH];
    uint8_t len = MIN_LENGTH;
};

Snake players[N_MAX_PLAYERS];

struct Snack {
    bool active;
    XY pos;
};

Snack snacks[N_MAX_SNACKS];
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

void makeSnack() {
    println(F("New snack"));
    Snack& sn = snacks[iSnack++];
    sn.active = true;
    sn.pos = XY::random();
    while (iSnack >= N_MAX_SNACKS)
        iSnack -= N_MAX_SNACKS;
}

void printPlayer(uint8_t ip) {
    print(F("P"));
    printRaw(ip);
    print(F(": "));
    Snake& p = players[ip];
    print(F("Dir "));
    printRaw((uint8_t)p.dir);
    print(F(", Pos "));
    for (uint8_t i = 0; i < p.len; i++) {
        XY pos = p.pos[i];
        printRaw((int16_t)pos.x);
        print(F(","));
        printRaw((int16_t)pos.y);
        print(F("|"));
    }
    print(F(" -> Len "));
    printRaw((uint8_t)p.len);
    println();
}

void move() {
    for (Snake& sn : players) {
        if (sn.dir == DEAD || sn.dir == OFF) continue;
        for (uint8_t i = sn.len - 1; i > 0; i--) {
            sn.pos[i] = sn.pos[i - 1];
        }
        sn.pos[0] = sn.pos[1] + dirToPos(sn.dir);
        sn.pos[0].wrap();
    }
}

bool collideSnakes(const XY& me, uint8_t myIndex) {
    for (uint8_t j = 0; j < N_MAX_PLAYERS; j++) {
        Snake& sn2 = players[j];
        // if (sn2.dir == DEAD) continue;
        for (uint8_t ip = 0; ip < sn2.len; ip++) {
            XY& pos = sn2.pos[ip];
            // skip collision check with own head
            if (myIndex == j && ip == 0) continue;
            if (me == pos) { return true; }
        }
    }
    return false;
}

bool collideSnack(const XY& me) {
    for (Snack& sn : snacks) {
        if (!sn.active) continue;
        if (me == sn.pos) {
            sn.active = false;
            return true;
        }
    }
    return false;
}

// expensive, but cleaner than comparing RGB buffers
void collide() {
    for (uint8_t i = 0; i < N_MAX_PLAYERS; i++) {
        Snake& sn = players[i];
        if (sn.dir == DEAD || sn.dir == OFF) continue;
        if (collideSnakes(sn.pos[0], i)) {
            sn.dir = DEAD;
            continue;
        }
        if (collideSnack(sn.pos[0])) {
            if (sn.len >= MAX_LENGTH) continue;
            sn.pos[sn.len] = sn.pos[sn.len - 1];
            sn.len += 1;
        }
    }
}

void draw() {
    Display::clear();
    for (uint8_t i = 0; i < N_MAX_PLAYERS; i++) {
        Snake& sn = players[i];
        CRGB head_color = C_PLAYERS[i];
        if (sn.dir == DEAD) { head_color = C_DEAD; }
        Display::screen->drawPixelRGB888(sn.pos[0].x, sn.pos[0].y, head_color.r, head_color.g, head_color.b);
        for (uint8_t j = 1; j < sn.len; j++) {
            CRGB color = head_color.fadeToBlackBy(i * (256 / (sn.len + 2) - 1));
            Display::screen->drawPixelRGB888(sn.pos[j].x, sn.pos[j].y, color.r, color.g, color.b);
        }
    }

    for (uint8_t i = 0; i < N_MAX_SNACKS; i++) {
        Snack& sn = snacks[i];
        if (!sn.active) continue;
        Display::screen->drawPixelRGB888(sn.pos.x, sn.pos.y, C_SNACK.r, C_SNACK.g, C_SNACK.b);
    }

    // printPlayer(0);
}

uint32_t nextSnack = 0;
void updateScreen() {
    move();
    collide();
    draw();

    uint32_t now = millis();
    if (now < nextSnack) return;
    nextSnack = now + SNACK_DELAY;
    makeSnack();
}

bool dirAllowed(const Direction& oldD, const Direction& newD) {
    if (newD == OFF || newD == DEAD) return false;
    if (oldD == OFF || oldD == DEAD) return true;

    const XY delta = dirToPos(oldD) + dirToPos(newD);
    return (abs(delta.x) == 1 && abs(delta.y) == 1);
}

} // namespace

void setDirection(uint8_t player, const Direction direction) {
    if (player >= N_MAX_PLAYERS) return;
    if (players[player].dir == DEAD || players[player].dir == direction) return;
    if (!dirAllowed(players[player].dir, direction)) return;
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
        XY pos = XY::random();
        for (uint8_t i = 0; i < sn.len; i++) {
            sn.pos[i] = pos;
        }
    }

    for (Snack& sn : snacks) {
        sn.active = OFF;
    }
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