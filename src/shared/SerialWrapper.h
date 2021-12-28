#pragma once

#include <Arduino.h>

#ifdef DEBUG
const bool USE_SERIAL = true;
#else
const bool USE_SERIAL = false;
#endif

namespace {
const uint8_t N_MAX_LOGS = 20;
const uint16_t T_HEARTBEAT = 10 * 1000;

struct RingBuffer {
    const __FlashStringHelper* log[N_MAX_LOGS] = {nullptr};
    uint8_t iLog = 0;
};

RingBuffer errors;
RingBuffer warnings;
} // namespace

// usually 115200 or 9600
void setupSerial(int baud) {
    if (!USE_SERIAL) return;

    Serial.begin(baud);
    Serial.println(); // init monitor
}

void heartbeatSerial() {
    if (!USE_SERIAL) return;

    static uint32_t lastCycle = millis();
    static uint32_t lastMsg = millis();

    uint32_t time = millis();

    if (time - lastMsg > T_HEARTBEAT) {
        Serial.print(F("cycle time: ["));
        Serial.print(time - lastCycle);
        Serial.println(F("]"));
        lastMsg = time;
    }

    lastCycle = millis();
}

// protected method to enforce "F" and ensure flag usage
void println(const __FlashStringHelper* string) {
    if (!USE_SERIAL) return;
    Serial.println(string);
}

void printlnRaw(String string) {
    if (!USE_SERIAL) return;
    Serial.println(string);
}

void printlnRaw(uint16_t number) {
    if (!USE_SERIAL) return;
    Serial.println(number);
}

void print(const __FlashStringHelper* string) {
    if (!USE_SERIAL) return;
    Serial.print(string);
}

void printRaw(String string) {
    if (!USE_SERIAL) return;
    Serial.print(string);
}

void printRaw(uint16_t number) {
    if (!USE_SERIAL) return;
    Serial.print(number);
}

void printArray(const uint8_t* rgb_array, uint8_t length) {
    if (!USE_SERIAL) return;
    Serial.print("[");
    for (uint8_t i = 0; i < length; i++) {
        Serial.print(rgb_array[i]);
        Serial.print(",");
    }
    Serial.println("]");
}

// ------ errors

void logWarning(const __FlashStringHelper* string) {
    if (warnings.iLog >= N_MAX_LOGS) {
        println(F("Warning list is full, wraparound"));
        warnings.iLog -= N_MAX_LOGS;
    }

    warnings.log[warnings.iLog++] = string;

    print(F("WARN: "));
    println(string);
}

void logError(const __FlashStringHelper* string) {
    if (errors.iLog >= N_MAX_LOGS) {
        println(F("Error list is full, wraparound"));
        errors.iLog -= N_MAX_LOGS;
    }

    errors.log[errors.iLog++] = string;

    print(F("ERROR: "));
    println(string);
}

const RingBuffer& getWarnLog() { return warnings; }

const RingBuffer& getErrorLog() { return errors; }
