#pragma once

#include "secrets.h"
#include <Arduino.h>
#include <UniversalTelegramBot.h> // https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <WiFiClientSecure.h>

namespace Input_Telegram {

const uint16_t POLL_DELAY = 5 * 1000; // min time, could be larger due to long running animations
const char* pairingKey = "hunter2";

WiFiClientSecure secured_client;
UniversalTelegramBot bot(T_BOT_TOKEN, secured_client);

void logMsg(const telegramMessage& msg) {
    print(F("T> "));
    printRaw(msg.from_name);
    print(F(" ("));
    printRaw(msg.from_id);
    print(F("): \""));
    printRaw(msg.text);
    println(F("\""));
}

// TODO pairing

void sendModeKeyboard(const String& chat_id) {
    bot.sendMessageWithReplyKeyboard(chat_id, F("Choose a command: "), "",
                                     F("[[\"/off\",\"/on\"], \"/clock\"]"), true);
}

void handleCommand(const String& chat_id, SafeString& cmd, SafeString& param1, SafeString& param2){
    if (cmd == "/pair"){
        if (param1 != pairingKey) {
            bot.sendMessage(chat_id, F("Sorry, that's not my key"));
            return;
        }
        bot.sendMessage(chat_id, F("Paired successfully!"));
        // TODO do something
    }
}

void handleCommand(const telegramMessage& msg) {
    if (msg.text == "/start" || msg.text == "/help") {
        // probably useless, Telegram offers native command selection now
        sendModeKeyboard(msg.chat_id);
    }

    // CMD PARAM1 PARAM2

    createSafeString(sMsg, 60);
    sMsg = msg.text.c_str();

    createSafeString(sCMD, 20);
    sMsg.nextToken(sCMD, " ");
    createSafeString(sPARAM1, 20);
    sMsg.nextToken(sPARAM1, " ");
    createSafeString(sPARAM2, 20);
    sMsg.nextToken(sPARAM2, " ");

    handleCommand(msg.chat_id, sCMD, sPARAM1, sPARAM2);
}

void handleCallBack(const telegramMessage& msg) {
    print(F("callback with "));
    printlnRaw(msg.text);
    // TODO handle
}

void handle(uint8_t n_msgs) {
    println(F("T:Recv msg, handling.."));

    for (int i = 0; i < n_msgs; i++) {
        telegramMessage msg = bot.messages[i];
        // TODO handle @BOTNAME postfixes
        msg.text.trim();

        logMsg(msg);
        bot.sendChatAction(msg.chat_id, "typing");

        if (msg.type == "callback_query")
            handleCallBack(msg);
        else if (msg.text.startsWith("/"))
            handleCommand(msg);
        else
            bot.sendMessage(msg.chat_id, F("?"));
    }
}

void notifyAdmin(const String& msg) {
    // or msg, but that's probably less performant
    bot.sendMessage(T_ADMIN_ID, msg, "");
}

bool ready = false;
// contract: WiFi must be enabled already
void setup() {
    print(F("Connecting to Telegram Bot API"));
    secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
    bot.waitForResponse = 4000;

    if (!bot.getMe()) {
        logWarning(F("T: Can't find myself!"));
    }

    notifyAdmin(F("I'm alive, feed me!"));
    ready = true;
}

uint32_t t_last_poll = 0;
void loop() {
    if (!ready)
        return;

    uint32_t now = millis();
    if (now - t_last_poll > POLL_DELAY) {
        println(F("T: Checking for updates..."));
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

        while (numNewMessages) {
            handle(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }

        t_last_poll = now;
    }
}

} // namespace Input_Telegram
