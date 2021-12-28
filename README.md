


https://cpp4arduino.com/2018/11/21/eight-tips-to-use-the-string-class-efficiently.html

F()-macro prevent preemptive init of "String" ref, use it!
Use const String& refs for passing them around. 
Use +=, which reduces allocations, ideally prefixed with .reserve(n). 
Use compile time concatenation with spaces. 

## TODO
Flash: self-reports 4MB, Uploader can only find 1MB. 

Error on captive portal: 
[E][WebServer.cpp:633] _handleRequest(): request handler not found
*wm:[2] <- Request redirected to captive portal

Implement all in/outputs


