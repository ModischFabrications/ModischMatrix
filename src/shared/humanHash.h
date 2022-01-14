#pragma once
#include <stdint.h>

// currently unused, but it might be useful for pairing later
// this class will make a hash without similar looking or sounding chars

const char* safeChars = "23456789ABCDEFGHKLMNPQRSTUVWXYZ";
const uint8_t base = sizeof(char*) / sizeof(safeChars);

// TODO include full https://github.com/RobTillaart/CRC
uint8_t reverse8(uint8_t in) {
    uint8_t x = in;
    x = (((x & 0xAA) >> 1) | ((x & 0x55) << 1));
    x = (((x & 0xCC) >> 2) | ((x & 0x33) << 2));
    x = ((x >> 4) | (x << 4));
    return x;
}

uint16_t reverse16(uint16_t in) {
    uint16_t x = in;
    x = (((x & 0XAAAA) >> 1) | ((x & 0X5555) << 1));
    x = (((x & 0xCCCC) >> 2) | ((x & 0X3333) << 2));
    x = (((x & 0xF0F0) >> 4) | ((x & 0X0F0F) << 4));
    x = ((x >> 8) | (x << 8));
    return x;
}

// CRC POLYNOME = x15 + 1 =  1000 0000 0000 0001 = 0x8001
uint16_t crc16(const uint8_t* array, uint8_t length, uint16_t polynome = 0x8001, uint16_t startmask = 0x0000,
               uint16_t endmask = 0x0000, bool reverseIn = false, bool reverseOut = false) {
    uint16_t crc = startmask;
    while (length--) {
        uint8_t data = *array++;
        if (reverseIn) data = reverse8(data);
        crc ^= ((uint16_t)data) << 8;
        for (uint8_t i = 8; i; i--) {
            if (crc & (1 << 15)) {
                crc <<= 1;
                crc ^= polynome;
            } else {
                crc <<= 1;
            }
        }
    }
    if (reverseOut) crc = reverse16(crc);
    crc ^= endmask;
    return crc;
}

// CRC is better than hash for intermediates, because it circumvents String issues.
uint16_t hash(const char* const input) { return crc16("Test123", 16); }

// TODO this is invalid, can't return address of temporaries
const char* const humanHash(uint16_t hash) {
    char hHash[12];
    uint8_t i = 0;
    while (hash != 0) {
        char c = safeChars[hash % base];
        hHash[i++] = c;
        hash /= base;
    }
    hHash[i] = '\0';
    return hHash;
}
