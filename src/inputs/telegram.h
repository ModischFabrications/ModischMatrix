#pragma once

#ifdef DEBUG
#define TELEGRAM_DEBUG
#endif

// Telegram Bot is technically a connector, especially with admin notifications. Might want to rename it.

#include "secrets.h"
#include "shared/serialWrapper.h"
#include <Arduino.h>
#include <UniversalTelegramBot.h> // https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <WiFiClientSecure.h>

namespace Input_Telegram {

const uint16_t POLL_DELAY = 5 * 1000; // min time, could be larger due to long running animations
const char* pairingKey = "hunter2";

WiFiClientSecure secured_client;
UniversalTelegramBot bot(T_BOT_TOKEN, secured_client);

const uint8_t N_MAX_PAIRED = 10;
const char* pairedKeys[N_MAX_PAIRED] = {nullptr};
uint8_t iPairs = 0;

void logMsg(const telegramMessage& msg) {
    print(F("T> "));
    printRaw(msg.from_name);
    print(F(" ("));
    printRaw(msg.from_id);
    print(F("): \""));
    printRaw(msg.text);
    println(F("\""));
}

void sendModeKeyboard(const String& chat_id) {
    bot.sendMessageWithReplyKeyboard(chat_id, F("Choose a command: "), "",
                                     F("[[\"/off\",\"/on\"], \"/clock\"]"), true);
}

void displayPairingKey() {
    print(F("Pairing Key: "));
    printlnRaw(pairingKey);
    // TODO show on display
}

void pairUser(const String& chat_id) {
    if (iPairs >= N_MAX_PAIRED)
        iPairs -= N_MAX_PAIRED;
    // add to Set of IDs
    pairedKeys[iPairs++] = chat_id.c_str();
}

bool isPaired(const String& chat_id) {
    print(F("Pairs: "));
    for (const char* i : pairedKeys) {
        printRaw(i);
        println(F(", "));
        if (chat_id.equals(i))
            return true;
    }
    return false;
}

void handleCommand(const String& chat_id, SafeString& cmd, SafeString& param1, SafeString& param2) {
    if (cmd == "/pair") {
        if (param1.isEmpty()) {
            bot.sendMessage(chat_id, F("Missing key, I expect \"/pair KEY\""));
        }
        if (param1 != pairingKey) {
            bot.sendMessage(chat_id, F("Sorry, that's not my key"));
            return;
        }
        pairUser(chat_id);
        bot.sendMessage(chat_id, F("Paired successfully!"));
    } else if (!isPaired(chat_id)) {
        bot.sendMessage(
            chat_id,
            F("I don't know you yet, let's pair! \nCall /pair KEY with the displayed characters"));
        displayPairingKey();
        return;
    }

    // everything is valid at this point
}

void handleCommand(const telegramMessage& msg) {
    if (msg.text.startsWith("/start") || msg.text.startsWith("/help")) {
        bot.sendMessage(
            msg.chat_id,
            F("Hello there 👋🏼 I'm here to give you easy access to ModischMatrix functions."));
        // probably useless, Telegram offers native command selection now
        sendModeKeyboard(msg.chat_id);
        return;
    }

    // TODO handle @BOTNAME postfixes
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
    println(F("Connecting to Telegram Bot API"));
    secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
    secured_client.setTimeout(5);
    // secured_client.setInsecure();   // this shouldn't be necessary with active Cert
    bot.waitForResponse = 5 * 1000;

    if (!bot.getMe()) {
        logWarning(F("T: Can't find myself!"));
    }

    // pairUser(T_ADMIN_ID);

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
