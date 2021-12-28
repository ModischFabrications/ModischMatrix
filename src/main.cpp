#include <Arduino.h>

#define DEBUG

#include "pinout.h"

#include "shared/SerialWrapper.h"
#include "shared/fileServer.h"
#include "shared/persistence/persistenceManager.h"
#include "shared/network/WiFiLoginManager.h"
//#include "shared/network/timeService.h"

#include "inputs/telegram.h"
#include "inputs/touch.h"
//#include "inputs/website.h"
#include "matrix/display.h"

void setup() {
    // prep watchdog
    delay(0);

    Pinout::setup();

    setupSerial(115200);
    println(F("Starting up..."));

    println(F(".prepare outputs."));
    Matrix::setup();

    println(F(".prepare connections."));
    WiFiLoginManager::setup("Matrix");

    println(F(".prepare inputs."));
    Input_Telegram::setup();
    Input_Touch::setup();

    println(F(".. Done!"));
}

void loop() {
    // impossible to align loop times with variable length web calls!
    //TimeService::timeUpdate();

    Input_Telegram::loop();

    PersistenceManager::trySave();

    heartbeatSerial();
}
