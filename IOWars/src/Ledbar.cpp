// Ledbar.cpp

#include "IOWarsWeiT.hpp"
#include "Stargate.hpp"
#include "Ledbar.hpp"
#include <iostream>

Ledbar::Ledbar(SPI* spiInstance)
    : spi(spiInstance) {

}


Ledbar::~Ledbar() {

}

void Ledbar::enable(bool state) {
    // STP_OE is P4.7, active low
    if (state) {
        spi->report->Bytes[4] &= ~(1 << 7);
    }
    else {
        spi->report->Bytes[4] |= (1 << 7);
    }

    IowKitWrite(*spi->DevHandle, IOW_PIPE_IO_PINS, (PCHAR)&spi->report, IOWKIT100_IO_REPORT_SIZE);
}

void Ledbar::latchEnable(bool state) {
    spi->enableSPI();
    spi->report->Bytes[10] &= ~(1 << 1); // !SS Low
    IowKitWrite(*spi->DevHandle, IOW_PIPE_IO_PINS, (PCHAR)&spi->report, IOWKIT100_IO_REPORT_SIZE);

    spi->report->Bytes[4] |= (1 << 0); // !LCD_ON High
    IowKitWrite(*spi->DevHandle, IOW_PIPE_IO_PINS, (PCHAR)&spi->report, IOWKIT100_IO_REPORT_SIZE);

    if (state) {
        spi->report->Bytes[5] &= ~0x70; // STP_LE HIGH
    }
    else {
        spi->report->Bytes[5] |= 0x70;// STP_LE Low
    }
    IowKitWrite(*spi->DevHandle, IOW_PIPE_IO_PINS, (PCHAR)&spi->report, IOWKIT100_IO_REPORT_SIZE);
}

void Ledbar::setAll(uint16_t pattern) {
    enable(false);

    latchEnable(false);

	spi->transfer16(pattern);

    latchEnable(true);

    latchEnable(false);
    
    enable(true);
}

void Ledbar::setSide(bool left, uint8_t pattern) {
    uint16_t fullPattern = 0x0000;

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
