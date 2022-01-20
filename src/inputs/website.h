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

const String* GetPostValue(AsyncWebServerRequest* request, const String& paramKey = F("value")) {
    if (!request->hasParam(paramKey, true)) {
        print(F("Missing param "));
        printlnRaw(paramKey);
        request->send(400, "text/plain", "missing param " + paramKey);
        Display::flashDot();
        return nullptr;
    }
    RebootManager::reset();
    return &(request->getParam(paramKey, true)->value());
}

// --- separate mode handlers

void GetUnknown(AsyncWebServerRequest* request) {
    request->send(404, "text/plain", "Not found");
    print(F("Unable to provide "));
    printlnRaw(request->url());
}

void PostBrightness(AsyncWebServerRequest* request) {
    const String* value = GetPostValue(request);
    if (value == nullptr) return;

    Controller::setBrightness(value->toInt());

    request->send(200, "text/plain", "OK");
    Display::flashDot();
}

void PostTimeout(AsyncWebServerRequest* request) {
    const String* value = GetPostValue(request);
    if (value == nullptr) return;

    Controller::hideAfter(value->toInt() * 1000);

    request->send(200, "text/plain", "OK");
    Display::flashDot();
}

void GetMode(AsyncWebServerRequest* request) {
    request->send(200, "text/plain", String(Controller::mode));
    Display::flashDot();
}

void PostMode(AsyncWebServerRequest* request) {
    const String* value = GetPostValue(request);
    if (value == nullptr) return;

    uint8_t val = value->toInt();
    Controller::setMode(val);

    request->send(200, "text/plain", "OK");
    Display::flashDot();
}

void PostPrint(AsyncWebServerRequest* request) {
    const String* value = GetPostValue(request);
    if (value == nullptr) return;
    Controller::setMode(Controller::Mode::STATIC);

    String val = *value;
    val.replace(F("\\n"), F("\n"));
    // TODO replace emoji and other special characters
    Controller::printText(val);

    request->send(200, "text/plain", "OK");
    Display::flashDot();
}

void PostSnake(AsyncWebServerRequest* request) {
    printlnRaw(request->pathArg(0));
    printlnRaw(request->pathArg(1));
    printlnRaw(request->pathArg(2));

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

    println(F("Preparing webserver..."));
    // cache for 10min
    server.serveStatic("/", SPIFFS, "/").setCacheControl("max-age=600").setDefaultFile("index.html");
    server.serveStatic("/favicon.ico", SPIFFS, "/favicon.png").setCacheControl("max-age=600");
    // server.onNotFound(GetUnknown);
    // server.on("/favicon.ico", HTTP_GET,
    //          [](AsyncWebServerRequest* request) { request->send(SPIFFS, "/favicon.png", "image/png"); });

    server.on("/brightness", HTTP_POST, PostBrightness);
    server.on("/timeout", HTTP_POST, PostTimeout);
    server.on("/mode", HTTP_GET, GetMode);
    server.on("/mode", HTTP_POST, PostMode);
    server.on("/print", HTTP_POST, PostPrint);
    server.on("/snake", HTTP_POST, PostSnake);

    server.begin();
    println(F("Serving website at '/"));
}

void loop() {}

} // namespace Website
