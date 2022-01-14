#pragma once

#include <Arduino.h>
#include <stdint.h>

#include "./configuration.h"
#include "./persistenceStore.h"
#include "shared/serialWrapper.h"

// definition for all handlers
typedef void (*fListener)();

using namespace Config;

// classes don't behave well with pointers, trust me!
namespace PersistenceManager {

const Configuration get();
void set(Configuration newConfig);
void trySave();
void registerListener(fListener listener);

// anonymous namespace hides globals
namespace {
void callListeners();

const uint8_t N_MAX_LISTENERS = 7;

const uint16_t delayToSaveMs = (60 * 1000);

// we are unable to determine if a variable was initialized and
// we don't want to define a "null" Configuration as default
bool initialized = false;

// it's dangerous to leave this uninitialized but we get these values with the first get()
Configuration configuration = defaultConfiguration;
uint32_t tNextSavepoint = 0;

fListener listeners[N_MAX_LISTENERS] = {nullptr};
uint8_t i_listeners = 0;

// -----------------------

void tryInit() {
    // singleton-like
    if (!initialized) {
        println(F("PM: Loading initial config from EEPROM"));
        configuration = PersistenceStore::loadSettings();
        initialized = true;
    }
}

// notify everyone interested that a new configuration is available
void callListeners() {
    tryInit();

    for (uint8_t i = 0; i < i_listeners; i++) {
        // print(F("Calling listener "));
        // printlnRaw(i);

        fListener listener = listeners[i];

        // check for value
        if (listener == nullptr) { println(F("Listener not initialised")); }
        // unpack function pointer from list and call
        (*listener)();
    }
}

} // namespace

// access current Configuration from EEPROM, including lazy load
const Configuration get() {
    tryInit();

    return configuration;
}

// write Configuration to EEPROM, lazy save after a small timeout to reduce EEPROM wear
void set(Configuration newConfig) {
    if (configuration == newConfig) {
        println(F("PM: config identical, skipping save"));
        return;
    }

    // reference too dangerous
    configuration = newConfig;

    // set "moving" timer to save as soon as user is done
    tNextSavepoint = (millis() + delayToSaveMs);

    callListeners();
}

/**
 * Call regularly to persist settings to EEPROM.
 * saves configuration to EEPROM if timeout has passed.
 * */
void trySave() {
    if (tNextSavepoint != 0 && millis() >= tNextSavepoint) {
        PersistenceStore::saveSettings(configuration);
        tNextSavepoint = 0;

        println(F("PM: Saving to EEPROM"));
    }
}

// Calls listeners automatically while loading a initial config
void registerListener(fListener listener) {
    println(F("PM: Adding listener"));

    if (i_listeners >= N_MAX_LISTENERS) {
        logError(F("PM: List is full, unable to add listener"));
        return;
    }

    listeners[i_listeners++] = listener;
}

void setup() { PersistenceStore::setup(); }

void loop() { trySave(); }

} // namespace PersistenceManager
