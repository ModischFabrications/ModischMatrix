[![PlatformIO CI](https://github.com/ModischFabrications/ModischMatrix/actions/workflows/pio.yml/badge.svg)](https://github.com/ModischFabrications/ModischMatrix/actions/workflows/pio.yml)

# ModischMatrix

This firmware is used to drive my custom made RGB-Matrix-Display. It's based on a ESP32 devboard and a commercial RGB Panel for the looks. 
They are connected through my (also custom made) PCB, see [HUB75_Driver_PCB](https://github.com/ModischFabrications/HUB75_Driver_PCB/). 
Everything is housed in a 3D-printed enclosure, see [HUB75_Panel_Enclosure](TODO).

I don't plan on selling (premade) kits yet, but feel free to contact for inquiries. 
Making everything yourself should set you back ~60€ and half a day of work, but requires some tools, parts and finesse.

Features are best described by the available API calls, see [API](#API).

## Usage

### First Setup
1. Build the hardware as referenced in the PCB manual 
2. Open Jumper, power Matrix via connector and ESP32 via USB
2. Checkout this repo and follow [Full Upload]
3. Watch serial output, try connecting to the new WiFi hotspot-> WiFiLoginPortal
4. If everything worked the hotspot should disappear and the new address should show up. Repeat if not.
5. Test some commands to the displayed address, look for visual feedback
5. Remove USB, connect jumper for final deployment


### Full Upload
VSCode should offer you a task to do everything automatically:
1. Close Serial Monitor
2. CTRL+ALT+T -> "deploy project"

Call "/deploy/deploy.sh" manually if that doesn't work or you don't believe in VSCode.
It still depends on platformIO, be aware that you might have to install the toolchain manually without VSCode.

Feel free to copy /website manually to /data if everything else fails; it's working, just not as efficient. 


## API

Use `modischmatrix.local` or the DHCP IP address to connect to the MatrixDisplay. Former is nicer, latter more robust, especially with static assignment.

More features are always added, check [enhancement issues](https://github.com/ModischFabrications/ModischMatrix/issues?q=is%3Aissue+is%3Aopen+label%3Aenhancement). 

Feel free to add more yourself!

Brightness, timeout and more can be appended to all commands. 

### Text
Blank spaces are usually resolved by your browser, force newlines with \n. Most special characters aren't supported, try your luck.

`http://modischmatrix.local/api?print=Hey%20there!\n%20Like%20it?\n%20YES%20%20NO%20%20&brightness=30`




## Dependencies 
See platformio.ini for details, should be handled automatically.

- https://github.com/me-no-dev/ESPAsyncWebServer : Can't recommend it enough, crazy how easy it can be. Way better than the native Webserver, try it yourself!
- https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA : Powerful, but complex. Read Readme thoroughly.
- https://github.com/FastLED/FastLED : *The* standard for high performance RGB pixel pushing. Low level, great control, great library!
- https://github.com/adafruit/Adafruit-GFX-Library : Base library for many complex UI libraries. Simple and high level, good for fast solutions. 
- https://github.com/tzapu/WiFiManager : Great solution for semi-automatic WiFi login. Step aside hard-coded credentials!
- https://github.com/bblanchon/ArduinoJson : Great library to parse JSON files. Not javascript, but simple enough and allegedly better performing than stdlib solution. 
- https://github.com/marian-craciunescu/ESP32Ping@^1.6 : Simple tool, does what it says. Somewhat unstable.
- ~~https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot : Great idea, sadly pretty unstable and performance-hungry.~~
- ~~https://github.com/PowerBroker2/SafeString : Midway between safe, but crazy complex c-arrays and easy, but unstable and expensive Arduino-Strings. Somewhat unusual documentation/environment, but responsive author. No library-integrations.~~
- https://github.com/tdewolff/minify/tree/master/cmd/minify : Used during deployment to minify the website; optional
- https://github.com/nickgammon/Regexp : Used to validate and parse web calls; great author, even without these libs


## References
These were useful while refining the concept, check them out as well if you are still shopping around: 

1. https://github.com/ModischFabrications/HUB75-DMA-Test \* 
1. https://github.com/ModischFabrications/Verdandi \* 
1. https://2dom.github.io/PixelTimes/
1. https://github.com/rorosaurus/esp32-hub75-driver
1. https://github.com/witnessmenow/ESP32-i2s-Matrix-Shield
1. https://www.instructables.com/Morphing-Digital-Clock/
1. https://github.com/bogd/esp32-morphing-clock
1. https://github.com/marcmerlin/AnimatedGIFs
1. https://github.com/LukPopp0/MatrixDisplay
1. https://randomnerdtutorials.com/esp32-touch-pins-arduino-ide/
1. https://github.com/witnessmenow/ESP8266-Led-Matrix-Web-Draw
1. https://arduinojson.org/v6/assistant/
1. All the examples of the libraries mentioned above

\* Great guy, love him like myself. 

Similar products, copy functionality: 
1. https://www.banggood.com/Cascadable-Dimmable-RGB-Full-Color-Voice-Activated-32+8-Dot-Matrix-Spectrum-Clock-Kit-Electronic-Production-DIY-Parts-p-1892932.html
2. ..? Might want to research more and extend this list.


## Design Choices

### Web Calls
I'm not good with HTML, so everything is based on simple calls. GET instead of POST is semantically worse, but much easier to use with generic web browsers. 

Feel free to improve my design!

### Deployment
Using a build script makes preprocessing the website much easier. 
Minifying, compression to gzip and upload to the device can be done as one step!

Packed data increases transmission speed and reduces memory usage. Unpacking happens automatically on all modern browsers, good thing that your device has x100 the power than this chip. 

### Shared Library
Many features persist between projects, so I try to keep a collection of important infrastructure. Might be extracted into a submodule eventually. 

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

Use raw `char*` formatting if possible, see [here](https://cpp4arduino.com/2020/02/07/how-to-format-strings-without-the-string-class.html).

SafeStrings should be an easier and safer alternative with automatic debugging and more. Pass by ref (&), but don't const them. Currently not used, haven't committed to it fully. 


### Telegram Bot
Functionally equivalent to web calls, but easier to make accessible. 
Problem: Bots are always reachable, but we don't want the matrix to be controlled by everyone. 

(A) Explicit whitelisting from admin: Cool, but manual labor and won't prevent out-of-house controls. 

(B) Pairing: Inspired by Android TV. Display pairing code on matrix, use as auth-key. 

Solution B is easier and usable without manual intervention. Allows timeout as well, makes cleanup easier. 

## Contributing
### CPP Formatting
I'm always happy about feedback, especially the ones with a solution as a pull request. 

Press Shift+Alt+F to apply formatting. 

This project using the Clang Formatter with a style configuration ´.clang-format´ file. A fitting extension should be recommended when checking this project out. 


## Known Issues
Touch2/Top seems to be connected to the onboard LED, can't use it for touch. See https://github.com/ModischFabrications/HUB75_Driver_PCB/issues/1 .
