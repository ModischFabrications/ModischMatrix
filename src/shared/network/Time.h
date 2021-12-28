#pragma once

#include <stdint.h>

// no namespace, just like configuration.h

// include this first if you get some weird "type not defined" errors
struct Time {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;

    const uint32_t toMillis() const {
        return ((((uint32_t)1 * this->hour * 60) + this->minute) * 60 + this->second) * 1000 +
               this->millisecond;
    }

    const bool operator==(const Time& other) const {
        return (this->hour == other.hour && this->minute == other.minute &&
                this->second == other.second && this->millisecond == other.millisecond);
    }

    const bool operator>(const Time& other) const { return this->toMillis() > other.toMillis(); }

    const bool operator<(const Time& other) const { return this->toMillis() < other.toMillis(); }

    const int64_t timeDifferenceInMs(const Time& other) const {
        return this->toMillis() - other.toMillis();
    }

    const String toString() {
        return String(this->hour) + ":" + String(this->minute) + ":" + String(this->second) + ":" +
               String(this->millisecond);
    }
};
