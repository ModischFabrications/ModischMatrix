[![PlatformIO CI](https://github.com/ModischFabrications/ModischMatrix/actions/workflows/pio.yml/badge.svg)](https://github.com/ModischFabrications/ModischMatrix/actions/workflows/pio.yml)

# ModischMatrix

***WIP!***


## Known Issues
Touch2/Top seems to be connected to the onboard LED, can't use it for touch. See https://github.com/ModischFabrications/HUB75_Driver_PCB/issues/1 .


Telegram Bot is technically a connector, especially with admin notifications. 

## Design Decisions

### Strings
Sure, cStrings (char[]) have great performance and memory safety, but I don't have 10 years time to learn and use them properly. 
Can't run from Strings forever, some libs require them. Keep their disadvantages in mind though!

Some best practices, stolen from [here](https://cpp4arduino.com/2018/11/21/eight-tips-to-use-the-string-class-efficiently.html) and [here](https://www.forward.com.au/pfod/ArduinoProgramming/ArduinoStrings/index.html):
- F()-macro prevent preemptive init of "String" ref, use it!
- Use const String& refs for passing them around. 
- Use +=, which reduces allocations, ideally prefixed with .reserve(n). 
- Use compile time concatenation with spaces. 
- local Strings seem to be okay
- reboot once in a while to recover heap

SafeStrings should be an easier and safer alternative with automatic debugging and more. Pass by ref (&), but don't const them. Currently not used, haven't committed to it fully. 


## Example Commands
Use `modischmatrix.local` or hardcoded IP address. Former is nicer, latter more robust.

`http://modischmatrix.local/api?print=Hey%20there!\n%20Like%20it?\n%20YES%20%20NO%20%20&brightness=30`


