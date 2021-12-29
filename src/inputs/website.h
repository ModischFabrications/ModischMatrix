#pragma once

#include "controller.h"
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
    print(F("Unable to provide "));
    printlnRaw(request->url());
}

void GetRoot(AsyncWebServerRequest* request) {
    request->send(200, "text/plain", "Hello there!");
    println(F("W> /"));
}

void GetAPI(AsyncWebServerRequest* request) {
    String message = F("Possible Commands, chain with \"&\":\n");
    message.reserve(120);
    if (request->params() <= 0) {
        // show possible commands
        message += F("/API?state=1\n?mode={0..4}\n?brightness={0..255}\n?print=TXT");
        request->send(200, "text/plain", message);
        return;
    }
    String paramKey;
    paramKey.reserve(20);
    paramKey = F("state");
    if (request->hasParam(paramKey)) {
        message += paramKey + "=";
        bool state = request->getParam(paramKey)->value().toInt() != 0;
        message += state;
        Controller::setVisibility(state);
    }
    paramKey = F("mode");
    if (request->hasParam(paramKey)) {
        message += paramKey + "=";
        uint8_t val = request->getParam(paramKey)->value().toInt();
        message += val;
        Controller::setMode(val);
    }
    paramKey = F("brightness");
    if (request->hasParam(paramKey)) {
        message += paramKey + "=";
        uint8_t val = request->getParam(paramKey)->value().toInt();
        message += val;
        Controller::setBrightness(val);
    }
    paramKey = F("print");
    if (request->hasParam(paramKey)) {
        message += paramKey + "=";
        const String val = request->getParam(paramKey)->value();
        message += val;
        Controller::print(val);
    }
    request->send(200, "text/plain", message);
    Display::flashBorder();
}

bool ready = false;
// contract: WiFi must be enabled already
void setup() {
    println(F("Preparing website"));
    server.on("/", HTTP_GET, GetRoot);
    server.on("/api", HTTP_GET, GetAPI);
    server.onNotFound(notFound);

    server.begin();
}

} // namespace Website
