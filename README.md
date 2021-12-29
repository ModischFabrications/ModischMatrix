
Telegram Bot is technically a connector, especially with admin notifications. 

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

WiFiManager only working after second restart, takes ~2s
another 2s needed until connection is established

Implement all in/outputs

fix formatter crash on new (v13) format

reboot every x Days ( with > 1h without activity)
set flag to restore UI in this case!
if (millis() > ONLINE_TIME && millis() - lastActivity > INACTIVE_TIME) ESP.restart();

