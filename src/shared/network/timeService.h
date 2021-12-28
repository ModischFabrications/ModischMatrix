#pragma once

// #include <NTPClient.h>
#include <WiFiUdp.h>
#include <coredecls.h>
#include <sntp.h>
#include <sys/time.h>
#include <time.h>

#include "TZ.h"
#include "Time.h"
#include "shared/persistence/persistenceManager.h"
#include "shared/SerialWrapper.h"

// overloading the function that controls the ntp update interval
uint32_t sntp_update_delay_MS_rfc_not_less_than_15000() {
    uint16_t interval = max(PersistenceManager::get().pollInterval, (uint16_t)1);
    return interval * 60 * 1000;
}

namespace TimeService {

bool initializedOffset = false;

void setup();
void timeUpdate();
const Time getCurrentTime();

namespace {

timeval tv;
Time currentTime = {0, 0, 0, 0};
uint16_t millisOffset = 0;

void updateConfiguration();
void initializeMillisOffset();

// initialize the millisecond offset to the ntp clock
// by waiting to the first change in seconds
void initializeMillisOffset() {
    gettimeofday(&tv, nullptr);
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    uint8_t initSecond = t->tm_sec;
    uint8_t nextSecond = initSecond;

    while (initSecond == nextSecond) {
        // please don't kill me watchdog
        delay(0);
        gettimeofday(&tv, nullptr);
        now = time(nullptr);
        t = localtime(&now);
        nextSecond = t->tm_sec;
    }

    // our measurement is never exactly precise!
    // we archieve +- 10ms
    millisOffset = 1000 - (millis() % 1000);

    println(F("Synchronized with NTP server"));
    initializedOffset = true;
}

// updates time values on configuration change
void updateConfiguration() {
    Config::Configuration config = PersistenceManager::get();
    configTime(config.timezone, "pool.ntp.org");

    print(F("Setting new timezone to "));
    printlnRaw(config.timezone);
    // pollInterval fetched live, no need to cache
}

} // namespace

// set up millisecond offset and register listener
void setup() {
    settimeofday_cb(initializeMillisOffset);
    PersistenceManager::registerListener(updateConfiguration);

    // updateConfiguration will be called automatically when registering
}

// every tick get and display new time
void timeUpdate() {
    if (!initializedOffset) {
        return;
    }

    // create new time object from current time
    Time newTime = currentTime;
    gettimeofday(&tv, nullptr);
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    newTime.hour = t->tm_hour;
    newTime.minute = t->tm_min;
    newTime.second = t->tm_sec;
    newTime.millisecond = (millis() + millisOffset) % 1000;

    // check for an overflow of each clock handle
    if (newTime.hour >= 24)
        logError(F("hour overflow"));
    if (newTime.minute >= 60)
        logError(F("minute overflow"));
    if (newTime.second >= 60)
        logError(F("second overflow"));
    if (newTime.millisecond >= 1000)
        logError(F("millisecond overflow"));

    // since the millisecond offset might not be perfect:
    // ignore and don't update time when tick happens to fall in that short window
    if (newTime.second == currentTime.second && newTime.millisecond < currentTime.millisecond) {
        // ~ every 5sec, we don't care that bad
        // logError(F("Non continuous time update, ignoring"));
        return;
    }

    currentTime = newTime;
}

const Time getCurrentTime() { return currentTime; }
} // namespace TimeService
