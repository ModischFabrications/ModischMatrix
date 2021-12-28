#pragma once

#include "shared/SerialWrapper.h"
#include <HTTPClient.h>

namespace analytics {
const char* host = "analytics.modisch.me";

void setup(const char* name) {
    HTTPClient http;
    http.begin(host);
    http.addHeader("Content-Type", "text/plain");
    uint16_t response = http.POST(name);
    if (response == 0) logWarning(F("Can't reach analytics!"));
}

} // namespace analytics
