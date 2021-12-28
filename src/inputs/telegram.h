#pragma once

#include "secrets.h"
#include <Arduino.h>
#include <UniversalTelegramBot.h> // https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <WiFiClientSecure.h>

namespace Input_Telegram {

const uint16_t POLL_DELAY = 10 * 1000; // min time, could be larger due to long running animations

WiFiClientSecure secured_client;
UniversalTelegramBot bot(T_BOT_TOKEN, secured_client);

void logMsg(const telegramMessage& msg) {
    print(F("T> "));
    printRaw(msg.from_name);
    print(F("("));
    printRaw(msg.from_id);
    print(F("): "));
    printlnRaw(msg.text);
}

// TODO pairing

void handle(uint8_t n_msgs) {
    println(F("T:Recv msg, handling.."));

    for (int i = 0; i < n_msgs; i++) {
        telegramMessage msg = bot.messages[i];
        msg.text.trim();

        logMsg(msg);
    }
}

void notifyAdmin(const String& msg) {
    // or msg, but that's probably less performant
    bot.sendMessage(T_ADMIN_ID, msg, "");
}

bool ready = false;
// contract: WiFi must be enabled already
void setup() {
    secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

    if (!bot.getMe()) { logWarning(F("T:Can't find myself!")); }

    notifyAdmin(F("I'm alive, feed me!"));
    ready = true;
}

uint32_t t_last_poll = 0;
void loop() {
    if (!ready) return;

    uint32_t now = millis();
    if (now - t_last_poll > POLL_DELAY) {
        println(F("Checking for updates..."));
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

        while (numNewMessages) {
            handle(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }

        t_last_poll = now;
    }
}

} // namespace Input_Telegram
