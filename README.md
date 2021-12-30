# ModischMatrix

***WIP!***
-----------

Touch2/Top seems to be connected to the onboard LED, can't use it for touch. 


Telegram Bot is technically a connector, especially with admin notifications. 
Telegram Bot is disabled for now, it's just too broken. 

## Strings
Sure, cStrings (char[]) have great performance and memory safety, but I don't have 10 years time to learn them. 
Can't run from Strings forever, some libs require them. Keep their disadvantages in mind though!

Some best practices, stolen from [here](https://cpp4arduino.com/2018/11/21/eight-tips-to-use-the-string-class-efficiently.html) and [here](https://www.forward.com.au/pfod/ArduinoProgramming/ArduinoStrings/index.html):
- F()-macro prevent preemptive init of "String" ref, use it!
- Use const String& refs for passing them around. 
- Use +=, which reduces allocations, ideally prefixed with .reserve(n). 
- Use compile time concatenation with spaces. 
- local Strings seem to be okay
- reboot once in a while to recover heap


SafeStrings should be an easier and safer alternative with automatic debugging and more. Pass by ref (&), but don't const them. 


## TODO
Flash: self-reports 4MB, Uploader can only find 1MB. 

Error on captive portal: 
[E][WebServer.cpp:633] _handleRequest(): request handler not found
*wm:[2] <- Request redirected to captive portal

12:31:36.599 > [E][ssl_client.cpp:36] _handle_error(): [start_ssl_client():216]: (-29312) SSL - The connection indicated an EOF
12:31:36.610 > [E][WiFiClientSecure.cpp:133] connect(): start_ssl_client: -29312

12:32:58.978 > [E][ssl_client.cpp:98] start_ssl_client(): Connect to Server failed!
12:32:58.988 > [E][WiFiClientSecure.cpp:133] connect(): start_ssl_client: -1

WiFiManager only working after second restart, takes ~2s
another 2s needed until connection is established

Implement all in/outputs

fix formatter crash on new (v13) format

reboot every x Days ( with > 1h without activity)
set flag to restore UI in this case!
if (millis() > ONLINE_TIME && millis() - lastActivity > INACTIVE_TIME) ESP.restart();

Update ESP core for telegram bot, see https://community.platformio.org/t/update-to-the-latest-version-v2-0-0-from-github/20739/2
Problem: HUB75 can't handle it somehow. 
