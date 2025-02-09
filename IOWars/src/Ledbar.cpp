// Ledbar.cpp

#include "IOWarsWeiT.hpp"
#include "Stargate.hpp"
#include "Ledbar.hpp"
#include <iostream>

Ledbar::Ledbar(SPI* spiInstance)
    : spi(spiInstance) {
    clear();  // Ensure LEDs are off at initialization
}

Ledbar::~Ledbar() {
    clear();  // Turn off LEDs on object destruction
}

void Ledbar::setAll(uint16_t pattern) {
    spi->transfer16(pattern);  // Send the 16-bit pattern using SPI
}

void Ledbar::setSide(bool left, uint8_t pattern) {
    uint16_t fullPattern;

    if (left) {
        // Place pattern in the high byte (left side)
        fullPattern = (pattern << 8);
    }
    else {
        // Place pattern in the low byte (right side)
        fullPattern = pattern;
    }

    setAll(fullPattern);
}

void Ledbar::clear() {
    setAll(0x0000);  // All LEDs off
}
