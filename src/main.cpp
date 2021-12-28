#include <Arduino.h>

#define DEBUG

#include "pinout.h"

#include "shared/SerialWrapper.h"
#include "shared/fileServer.h"
#include "shared/network/WiFiLoginManager.h"
#include "shared/persistence/persistenceManager.h"
//#include "shared/network/timeService.h"

#include "inputs/telegram.h"
#include "inputs/touch.h"
#include "inputs/website.h"

#include "matrix/display.h"

void setup() {
    // prep watchdog
    delay(0);

    Pinout::setup();
    digitalWrite(Pinout::STATUS_LED, true);

    setupSerial(115200);
    println(F("Starting up..."));
    PersistenceStore::setup();

    println(F(".prepare outputs."));
    Display::setup();

    println(F(".prepare connections."));
    WiFiLoginManager::setup("ModischMatrix");

    println(F(".prepare inputs."));
    Input_Telegram::setup();
    Input_Touch::setup();

    println(F(".. Done!"));
    digitalWrite(Pinout::STATUS_LED, false);
}

void loop() {
    // impossible to align loop times with variable length web calls!

    Input_Telegram::loop();

    PersistenceManager::trySave();

    heartbeatSerial();
}
