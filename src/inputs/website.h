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

void GetRoot(AsyncWebServerRequest* request) {
    request->send(200, "text/plain", "Hello there! Check /api for commands");
    println(F("W> /"));
}

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

void GetPrint(AsyncWebServerRequest* request) {
    if (!request->hasParam(F("msg"))) {
        request->send(400, "text/plain", F("/print?msg=Row1\nRow2"));
        Display::flashDot();
        return;
    }
    RebootManager::reset();
    Controller::setMode(Controller::Mode::STATIC);

    String val = request->getParam(F("msg"))->value();
    val.replace(F("\\n"), F("\n"));
    // TODO replace emoji and other special characters
    Controller::printText(val);

    request->send(200, "text/plain", "OK");
    Display::flashDot();
}

void GetSnake(AsyncWebServerRequest* request) {
    if (!request->hasParam(F("p")) || !request->hasParam(F("dir"))) {
        request->send(400, "text/plain", F("/snake?p={0..3}&dir={0..3}"));
        Display::flashDot();
        return;
    }
    RebootManager::reset();
    Controller::setMode(Controller::Mode::SNAKE);

    uint8_t player = request->getParam(F("p"))->value().toInt();
    Modes_Snake::Direction direction = (Modes_Snake::Direction)request->getParam(F("dir"))->value().toInt();
    Modes_Snake::setDirection(player, direction);

    request->send(200, "text/plain", "OK");
    Display::flashDot();
}
} // namespace

// contract: WiFi must be enabled already
void setup() {
    println(F("Preparing website"));
    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
    server.on("/api", HTTP_GET, GetAPI);
    server.on("/print", HTTP_GET, GetPrint);
    server.on("/snake", HTTP_GET, GetSnake);
    server.on("/favicon.ico", HTTP_GET,
              [](AsyncWebServerRequest* request) { request->send(SPIFFS, "/favicon.png", "image/png"); });

    server.onNotFound(GetUnknown);

    server.begin();
}

void loop() {}

} // namespace Website
