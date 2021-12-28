#pragma once

#include "secrets.h"
#include <Arduino.h>
#include <UniversalTelegramBot.h> // https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <WiFiClientSecure.h>

// TODO technically a connector, especially with admin notifications
namespace Input_Telegram {

const uint16_t POLL_DELAY = 2000; // min time, could be larger due to long running animations
// const X509List cert(TELEGRAM_CERTIFICATE_ROOT);

WiFiClientSecure secured_client;
UniversalTelegramBot bot(T_BOT_TOKEN, secured_client);

void handle(uint8_t n_msgs) {
    println(F("T:Recv msg, handling.."));

    for (int i = 0; i < n_msgs; i++) {
        String chat_id = bot.messages[i].chat_id;
        String text = bot.messages[i].text;
        String from_name = bot.messages[i].from_name;

        if (from_name == "")
            from_name = "Guest";

        print(F("chat_id: "));
        printRaw(chat_id);
        print(F(", text: "));
        printRaw(text);
        print(F(", from_name: "));
        printRaw(from_name);
    }
}

void notifyAdmin(const String& msg) {
    // or msg, but that's probably less performant
    bot.sendMessage(T_ADMIN_ID, msg, "");
}

// contract: WiFi must be enabled already
void setup() {
    secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

    if (!bot.getMe()) {
        logError(F("T:Can't find myself!"));
        return;
    }

    notifyAdmin(F("I'm alive, feed be!"));
}

uint32_t t_last_poll = 0;
void loop() {
    uint32_t now = millis();
    if (now - t_last_poll > POLL_DELAY) {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

        while (numNewMessages) {
            handle(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }

        t_last_poll = now;
    }
}

} // namespace Input_Telegram
