#pragma once

#include <Arduino.h>
#include <UniversalTelegramBot.h> // https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <WiFiClientSecure.h>

namespace Input_Telegram {

const String BOT_TOKEN = "TODO CHANGE ME";
const uint16_t POLL_DELAY = 2000; // min time, could be larger due to long running animations
// const X509List cert(TELEGRAM_CERTIFICATE_ROOT);

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

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

// contract: WiFi must be enabled already
void setup() {
    if (!bot.getMe()){
        logError(F("T:Can't find myself!"));
    }
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
