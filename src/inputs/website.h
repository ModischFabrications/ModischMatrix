#pragma once

#include "shared/serialWrapper.h"
#include <Arduino.h>
#include <ESPAsyncWebServer.h> // https://github.com/me-no-dev/ESPAsyncWebServer

/**
 * /cmd?state=1
 *
 */

namespace Website {

AsyncWebServer server(80);

void notFound(AsyncWebServerRequest* request) {
    request->send(404, "text/plain", "Not found");
    print(F("Unable to provide /"));
    printlnRaw(request->url());
}

bool ready = false;
// contract: WiFi must be enabled already
void setup() {
    println(F("Preparing website"));
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(200, "text/plain", "Hello there!");
        println(F("W> /"));
    });

    server.on("/cmd", HTTP_GET, [](AsyncWebServerRequest* request) {
        String message = F("CMD: ");
        if (request->hasParam(F("state"))) {
            message += F("state=");
            message += request->getParam(F("state"))->value();
        } else {
            message = F("No message sent");
        }
        request->send(200, "text/plain", message);
    });

    server.onNotFound(notFound);

    server.begin();
}

} // namespace Website
