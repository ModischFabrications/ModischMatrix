#pragma once

// Shouldn't be included manually, use Manager!

// https://github.com/esp8266/Arduino/tree/master/libraries/EEPROM
#include <EEPROM.h>
//#include <LITTLEFS.h>

#include "./configuration.h"

// MAKE SURE TO DEFINE VERSION IN YOUR CONFIG FILE!

namespace PersistenceStore {
// Preserve EEPROM while programming on AVR with the EESAVE fuse

using namespace Config;

void setup();
void saveSettings(const Configuration settings);
const Configuration loadSettings();

namespace {

/**
 * EEPROM has around 100k writes per cell, so use them carefully!
 * It might be a good idea to switch to new cells with each revision.
 * ATtiny has 512 bytes EEPROM.
 *
 * Wear leveling is logically impossible to automate without external
 * input, e.g. version numbers.
 * Storage pointers burn out the pointer cell, random won't be found
 * after a restart.
 * */
const uint16_t EEPROM_VERSION_ADDR = VERSION;
const uint16_t EEPROM_SETTINGS_ADDR = EEPROM_VERSION_ADDR + sizeof(VERSION);

#if defined(ESP32) || defined(ESP8266)
// ESP needs to know how much storage we actually need, AVR won't
const uint16_t MAX_BYTES_USED = 127;
#endif
} // namespace

// call once at startup to reserve (emulated) storage for usage
void setup() {
#if defined(ESP32) || defined(ESP8266)
    EEPROM.begin(MAX_BYTES_USED);
#endif
}

// save settings to EEPROM for persistent storage
void saveSettings(const Configuration settings) {
    EEPROM.write(EEPROM_VERSION_ADDR, VERSION);
    EEPROM.put(EEPROM_SETTINGS_ADDR, settings);
#if defined(ESP32) || defined(ESP8266)
    // commit planned changes
    EEPROM.commit();
#endif
}

/**
 * load settings from EEPROM, this will reset and return default values for
 * corrupted, outdated or missing settings.
 * The chance that a random, unset combination is a match is very low.
 * */
const Configuration loadSettings() {
    // could be rubbish or zeros, either way should work
    uint8_t savedVersion = EEPROM.read(EEPROM_VERSION_ADDR);

    if (savedVersion != VERSION) {
        // save new default settings as fallback, updates version for next run
        saveSettings(defaultConfiguration);

        return defaultConfiguration;
    }

    // default isn't actually used, but it prevents compile warnings
    Configuration settings = defaultConfiguration;
    EEPROM.get(EEPROM_SETTINGS_ADDR, settings);

    return settings;
}

} // namespace PersistenceStore