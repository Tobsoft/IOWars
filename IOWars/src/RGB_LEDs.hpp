// RGB_LEDs.hpp
// Class for controlling WS2812 RGB LEDs
// DIN of first LED is P2.1

#ifndef RGB_LEDS_H
#define RGB_LEDS_H

#include <cstring>
#include <stdexcept>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdint>

#define NUM_LEDS 12


#define LED_TYPE_SK2812 0
#define LED_TYPE_LC8812 1
#define LED_TYPE_WS2812B 2
#define LED_TYPE_WS2812 3
#define LED_TYPE_WS2813 4

#define LED_TYPE LED_TYPE_WS2812B

/*
* Signaling Times
* 0Cyc...length of zero bitcell
* 0Pha...length of high phase for zero bit
* 1Cyc...length of one bitcell
* 1Pha...length of high phase for one bit
*/
#define WS2812B_0CYC 0x3C
#define WS2812B_0PHA 0x13
#define WS2812B_1CYC 0x3C
#define WS2812B_1PHA 0x26

#define SK_2812_0CYC 0x39
#define SK_2812_0PHA 0x0E
#define SK_2812_1CYC 0x39
#define SK_2812_1PHA 0x1C

#define LC8812_0CYC 0x2B
#define LC8812_0PHA 0x0E
#define LC8812_1CYC 0x48
#define LC8812_1PHA 0x2B

#define WS2812_0CYC 0x37
#define WS2812_0PHA 0x10
#define WS2812_1CYC 0x3E
#define WS2812_1PHA 0x21

#define WS2813_0CYC 0x45
#define WS2813_0PHA 0x10
#define WS2813_1CYC 0x45
#define WS2813_1PHA 0x34


#if LED_TYPE == LED_TYPE_WS2812B
#define _0CYC WS2812B_0CYC
#define _0PHA WS2812B_0PHA
#define _1CYC WS2812B_1CYC
#define _1PHA WS2812B_1PHA

#elif LED_TYPE == LED_TYPE_SK_2812
#define _0CYC SK_2812_0CYC
#define _0PHA SK_2812_0PHA
#define _1CYC SK_2812_1CYC
#define _1PHA SK_2812_1PHA

#elif LED_TYPE == LED_TYPE_LC8812
#define _0CYC LC8812_0CYC
#define _0PHA LC8812_0PHA
#define _1CYC LC8812_1CYC
#define _1PHA LC8812_1PHA

#elif LED_TYPE == LED_TYPE_WS2812
#define _0CYC WS2812_0CYC
#define _0PHA WS2812_0PHA
#define _1CYC WS2812_1CYC
#define _1PHA WS2812_1PHA

#elif LED_TYPE == LED_TYPE_WS2813
#define _0CYC WS2813_0CYC
#define _0PHA WS2813_0PHA
#define _1CYC WS2813_1CYC
#define _1PHA WS2813_1PHA

#else
#error("Invalid LED Type. Use #define LED_TYPE to set the LED type.")
#endif

class RGB_LEDs {
public:
    RGB_LEDs(IOWKIT_HANDLE* deviceHandle, IOWKIT100_IO_REPORT* report);
    ~RGB_LEDs();

    void enable(bool state);
    void setLEDColor(int ledIndex, uint8_t red, uint8_t green, uint8_t blue);
    void updateLEDs();

private:
    IOWKIT_HANDLE* DevHandle;         // Reference to IOWarrior handle
    IOWKIT100_IO_REPORT* report;      // Shared report reference
    IOWKIT100_SPECIAL_REPORT LED_report;
    uint8_t ledBuffer[NUM_LEDS * 3];  // RGB values for each LED
};

#endif // RGB_LEDS_H
