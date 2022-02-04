#pragma once

#include <Arduino.h>

// milli-less timings, cheap and simple. No cocurrency, parallel flows or similar async!
class SimpleTimer {
  private:
    uint32_t t_delayed_call = 0;
    void (*delayed_call)() = nullptr;

  public:
    void registerCall(void (*call)(), uint32_t delay) {
        if (delayed_call != nullptr) delayed_call();
        t_delayed_call = millis() + delay;
        delayed_call = call;
    }

    void loop() {
        if (t_delayed_call == 0 || millis() < t_delayed_call || delayed_call == nullptr) return;
        t_delayed_call = 0;
        delayed_call();
        delayed_call = nullptr;
    }
};
