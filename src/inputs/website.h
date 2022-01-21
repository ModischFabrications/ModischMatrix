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

    const String& val = request->getParam(paramKey, true)->value();
    print(F("Post value: "));
    printlnRaw(val);

    return &(val);
}

String GetPathArgument(const String& path) {
    // skip first entry from base url
    uint8_t i_slash = path.indexOf("/", 1);
    uint8_t i_slash2 = path.indexOf("/", i_slash + 1);
    if (i_slash2 == -1) i_slash2 = path.length();

    const String arg = path.substring(i_slash + 1, i_slash2);
    print(F("Path arg: "));
    printlnRaw(arg);

    return arg;
}

// --- separate mode handlers

void RedirectUnknown(AsyncWebServerRequest* request) {
    request->redirect("/");
    print(F("Redirecting request to root from "));
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

void GetNSnakes(AsyncWebServerRequest* request) {
    request->send(200, "text/plain", String(Modes_Snake::N_MAX_PLAYERS));
    Display::flashDot();
}

// snake/0[/]
void PostSnake(AsyncWebServerRequest* request) {
    uint8_t player = GetPathArgument(request->url()).toInt();
    const String* value = GetPostValue(request);
    if (value == nullptr || player < 0) return;

    Controller::setMode(Controller::Mode::SNAKE);
    Modes_Snake::setDirection(player, (Modes_Snake::Direction)value->toInt());

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
    // needed to fetch css/js, pictures and more; might be deobfuscation risk for hidden files
    server.serveStatic("/", SPIFFS, "/").setCacheControl("max-age=600").setDefaultFile("index.html");
    server.serveStatic("/favicon.ico", SPIFFS, "/favicon.png").setCacheControl("max-age=600");
    server.onNotFound(RedirectUnknown);

    server.on("/brightness", HTTP_POST, PostBrightness);
    server.on("/timeout", HTTP_POST, PostTimeout);
    server.on("/mode", HTTP_GET, GetMode);
    server.on("/mode", HTTP_POST, PostMode);
    server.on("/print", HTTP_POST, PostPrint);
    server.on("/snake", HTTP_GET, GetNSnakes);
    server.on("/snake", HTTP_POST, PostSnake);

    server.begin();
    println(F("Serving website at '/"));
}

void loop() {}

} // namespace Website
