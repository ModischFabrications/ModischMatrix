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
    request->send(200, "text/plain", "Hello there! Check /api for commands");
    println(F("W> /"));
}

void GetAPI(AsyncWebServerRequest* request) {
    RebootManager::setActive();

    String message = F("Possible Commands, chain with \"&\":\n");
    message.reserve(200);
    // always show possible commands
    message += F("/api?\n?mode={0..4}\n?brightness={0..255}\n?print=TXT\n?timeout=SECONDS\n\n");
    if (request->params() <= 0) {
        request->send(200, "text/plain", message);
        return;
    }
    message += F("OK: \n");

    // TODO streamline
    String paramKey;
    paramKey.reserve(20);
    paramKey = F("mode");
    if (request->hasParam(paramKey)) {
        message += "\n" + paramKey + "=";
        uint8_t val = request->getParam(paramKey)->value().toInt();
        message += val;
        Controller::setMode(val);
    }
    paramKey = F("brightness");
    if (request->hasParam(paramKey)) {
        message += "\n" + paramKey + "=";
        uint8_t val = request->getParam(paramKey)->value().toInt();
        message += val;
        Controller::setBrightness(val);
    }
    paramKey = F("print");
    if (request->hasParam(paramKey)) {
        message += "\n" + paramKey + "=";
        String val = request->getParam(paramKey)->value();
        message += val;
        val.replace(F("\\n"), F("\n"));
        Controller::printText(val);
    }
    paramKey = F("timeout");
    if (request->hasParam(paramKey)) {
        message += "\n" + paramKey + "=";
        uint16_t val = request->getParam(paramKey)->value().toInt();
        message += val;
        Controller::hideAfter(val * 1000);
    }
    request->send(200, "text/plain", message);
    Display::flashDot();
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
