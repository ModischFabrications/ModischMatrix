#pragma once

#include "controller.h"
#include "modes/snake.h"
#include "shared/serialWrapper.h"
#include <Arduino.h>
#include <ESPAsyncWebServer.h> // https://github.com/me-no-dev/ESPAsyncWebServer
#include <SPIFFS.h>

namespace Website {

namespace {
AsyncWebServer server(80);

void GetUnknown(AsyncWebServerRequest* request) {
    request->send(404, "text/plain", "Not found");
    print(F("Unable to provide "));
    printlnRaw(request->url());
}

// TODO make brightness & timeout standalones, no need to combine
void GetAPI(AsyncWebServerRequest* request) {
    RebootManager::reset();

    String message = F("Possible Commands, chain with \"&\":\n");
    message.reserve(200);
    // always show possible commands
    message += F("/api?\n?mode={0..?}\n?brightness={0..255}\n?timeout=SECONDS\n\n");
    if (request->params() <= 0) {
        request->send(400, "text/plain", message);
        Display::flashDot();
        return;
    }
    message += F("OK: \n");

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

// --- separate mode handlers

void GetMode(AsyncWebServerRequest* request) {
    request->send(200, "text/plain", String(Controller::mode));
    Display::flashDot();
}

void PostMode(AsyncWebServerRequest* request) {
    String paramKey = F("value");

    if (!request->hasParam(paramKey, true)) {
        print(F("Missing param "));
        printlnRaw(paramKey);
        request->send(400, "text/plain", "missing param " + paramKey);
        Display::flashDot();
        return;
    }
    RebootManager::reset();

    uint8_t val = request->getParam(paramKey, true)->value().toInt();
    Controller::setMode(val);

    request->send(200, "text/plain", "OK");
    Display::flashDot();
}

void PostPrint(AsyncWebServerRequest* request) {
    if (!request->hasParam(F("value"), true)) {
        request->send(400, "text/plain", F("/print?value=Row1\nRow2"));
        Display::flashDot();
        return;
    }
    RebootManager::reset();
    Controller::setMode(Controller::Mode::STATIC);

    String val = request->getParam(F("value"), true)->value();
    val.replace(F("\\n"), F("\n"));
    // TODO replace emoji and other special characters
    Controller::printText(val);

    request->send(200, "text/plain", "OK");
    Display::flashDot();
}

void PostSnake(AsyncWebServerRequest* request) {
    if (!request->hasParam(F("p"), true) || !request->hasParam(F("dir"), true)) {
        request->send(400, "text/plain", F("/snake?p={0..3}&dir={0..3}"));
        Display::flashDot();
        return;
    }
    RebootManager::reset();
    Controller::setMode(Controller::Mode::SNAKE);

    uint8_t player = request->getParam(F("p"), true)->value().toInt();
    Modes_Snake::Direction direction = (Modes_Snake::Direction)request->getParam(F("dir"), true)->value().toInt();
    Modes_Snake::setDirection(player, direction);

    request->send(200, "text/plain", "OK");
    Display::flashDot();
}
} // namespace

// contract: WiFi and modes must be enabled already
void setup() {
    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    println(F("Preparing website"));
    server.serveStatic("/", SPIFFS, "/").setCacheControl("max-age=600").setDefaultFile("index.html");
    server.on("/api", HTTP_GET, GetAPI);
    server.on("/mode", HTTP_GET, GetMode);
    server.on("/mode", HTTP_POST, PostMode);
    server.on("/print", HTTP_POST, PostPrint);
    server.on("/snake", HTTP_POST, PostSnake);
    server.on("/favicon.ico", HTTP_GET,
              [](AsyncWebServerRequest* request) { request->send(SPIFFS, "/favicon.png", "image/png"); });

    server.onNotFound(GetUnknown);

    server.begin();
}

void loop() {}

} // namespace Website
