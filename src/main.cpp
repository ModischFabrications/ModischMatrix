#include <Arduino.h>
//#include <loopTimer.h>    // part of SafeStrings

#define DEBUG

#ifdef CI_BUILD
#pragma message "-- CI Build --"
#endif

#include "pinout.h"

#include "shared/fileServer.h"
#include "shared/network/WiFiLoginManager.h"
#include "shared/persistence/persistenceManager.h"
#include "shared/serialWrapper.h"
#include "shared/timeService.h"
#include "shared/weatherService.h"

//#include "inputs/telegram.h"
#include "inputs/touch.h"
#include "inputs/website.h"

#include "controller.h"
#include "display.h"

void setup() {
    delay(0); // watchdog timer (WDT)

    Pinout::setup();
    digitalWrite(Pinout::STATUS_LED, true);

    setupSerial(115200);
    println(F("Starting up, this will take ~5 seconds..."));
    PersistenceManager::setup();

    println(F(".prepare outputs."));
    Display::setup();
    Display::colorDot(Display::blue); // asap

    println(F(".prepare connections."));
    WiFiLoginManager::onConfigNeeded = [] { Display::colorDot(Display::orange); };
    WiFiLoginManager::setup("ModischMatrix");
    delay(10);

    println(F(".prepare inputs."));
    TimeService::setup();
    WeatherService::setup();
    Website::setup();
    Input_Touch::setup();
    // Input_Telegram::setup();

    println(F(".prepare logic."));
    Controller::setup();

    PersistenceManager::callListeners();

    println(F(".. Done!"));
    digitalWrite(Pinout::STATUS_LED, false);
    Display::colorDot(Display::black);
    delay(10);
}

void loop() {
    TimeService::loop();
    WeatherService::loop();
    Input_Touch::loop();
    Controller::loop();

    PersistenceManager::loop();

    // Input_Telegram::loop();

    RebootManager::loop();

    // loopTimer.check(Serial);
    // heartbeatSerial();
    delay(1);
}
