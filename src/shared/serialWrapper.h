#pragma once

#include <Arduino.h>

#ifdef DEBUG
#pragma message("-- DEBUG Build, enabling serial output --")
const bool USE_SERIAL = true;
#else
#pragma message("-- RELEASE Build, disabling serial output --")
const bool USE_SERIAL = false;
#endif

namespace {
const uint16_t T_HEARTBEAT = 10L * 1000;

#ifdef SERIAL_LOGGING
#pragma message("-- SERIAL_LOGGING set, enabling warning & errors logs --")
const uint8_t N_MAX_LOGS = 20;

struct RingBuffer {
    const __FlashStringHelper* log[N_MAX_LOGS] = {nullptr};
    uint8_t iLog = 0;
};

RingBuffer errors;
RingBuffer warnings;
#else
#pragma message("-- SERIAL_LOGGING not set, disabling warning & errors logs --")
#endif
} // namespace

// usually 115200 or 9600
void setupSerial(uint32_t baud) {
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
        Serial.print(F("avg cycle time: ~"));
        Serial.print(time - lastCycle);
        Serial.println(F("ms"));
        lastMsg = time;
    }

    lastCycle = time;
}

void println() {
    if (!USE_SERIAL) return;
    Serial.println();
}

// protected method to enforce "F" and ensure flag usage
void println(const __FlashStringHelper* string) {
    if (!USE_SERIAL) return;
    Serial.println(string);
}

void printlnRaw(const String& string) {
    if (!USE_SERIAL) return;
    Serial.println(string);
}

void printlnRaw(int16_t number) {
    if (!USE_SERIAL) return;
    Serial.println(number);
}

void printlnRaw(uint16_t number) {
    if (!USE_SERIAL) return;
    Serial.println(number);
}

void printlnRaw(uint8_t number) {
    if (!USE_SERIAL) return;
    Serial.println(number);
}

void printlnRaw(float number) {
    if (!USE_SERIAL) return;
    Serial.println(number);
}

void print(const __FlashStringHelper* string) {
    if (!USE_SERIAL) return;
    Serial.print(string);
}

void printRaw(const String& string) {
    if (!USE_SERIAL) return;
    Serial.print(string);
}

void printRaw(int16_t number) {
    if (!USE_SERIAL) return;
    Serial.print(number);
}

void printRaw(uint16_t number) {
    if (!USE_SERIAL) return;
    Serial.print(number);
}

void printRaw(uint8_t number) {
    if (!USE_SERIAL) return;
    Serial.print(number);
}

void printRaw(float number) {
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

// ------ warnings & errors

void logWarning(const __FlashStringHelper* string) {
    print(F("WARN: "));
    println(string);
#ifdef SERIAL_LOGGING
    if (warnings.iLog >= N_MAX_LOGS) {
        println(F("Warning list is full, wraparound"));
        warnings.iLog -= N_MAX_LOGS;
    }

    warnings.log[warnings.iLog++] = string;
#endif
}

void logError(const __FlashStringHelper* string) {
    print(F("ERROR: "));
    println(string);
#ifdef SERIAL_LOGGING
    if (errors.iLog >= N_MAX_LOGS) {
        println(F("Error list is full, wraparound"));
        errors.iLog -= N_MAX_LOGS;
    }

    errors.log[errors.iLog++] = string;
#endif
}

#ifdef SERIAL_LOGGING
const RingBuffer& getWarnLog() { return warnings; }

const RingBuffer& getErrorLog() { return errors; }
#endif
