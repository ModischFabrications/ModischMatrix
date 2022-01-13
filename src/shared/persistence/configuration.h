#pragma once

#include <stdint.h>

namespace Config {
namespace {
/**
 * change with each design iteration to prevent EEPROM inconsistency and help
 * with wear leveling of EEPROM cells.
 * */
const uint8_t VERSION = 2;
} // namespace

struct Configuration {
    uint8_t mode;
    uint8_t brightness;

    const bool operator==(const Configuration& other) {
        return (this->mode == other.mode && this->brightness == other.brightness);
    }
};

const Configuration defaultConfiguration = {2, 64};

} // namespace Config