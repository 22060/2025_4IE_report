#pragma once
#include <Arduino.h>
class tinytasker
{
public:
    tinytasker(uint16_t interval, void (*func)()) : interval(interval), func(func) {};
    void update()
    {
        if (millis() - last > interval)
        {
            last = millis();
            func();
        }
    }

private:
    uint16_t interval = 0;
    void (*func)();
    uint32_t last = 0;
};
