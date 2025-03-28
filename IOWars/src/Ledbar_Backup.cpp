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

void Ledbar::setAll(uint16_t pattern) {
    spi->report->Bytes[4] |= (1<<0); // !LCD-ON High
    IowKitWrite(*spi->DevHandle, IOW_PIPE_IO_PINS, (PCHAR)&spi->report, IOWKIT100_IO_REPORT_SIZE);

    spi->enableSPI();
    spi->report->Bytes[10] &= ~(1 << 1); // !SS low

    spi->report->Bytes[5] &= ~0x70; // D14B->Q1 High; STP_LE High
    IowKitWrite(*spi->DevHandle, IOW_PIPE_IO_PINS, (PCHAR)&spi->report, IOWKIT100_IO_REPORT_SIZE);
    std::this_thread::sleep_for(std::chrono::microseconds(10));

    // Ensure LE (latch enable) is low before shifting data
	spi->report->Bytes[5] |= 0x70; // D14B->Q1 Low; STP_LE Low
    IowKitWrite(*spi->DevHandle, IOW_PIPE_IO_PINS, (PCHAR)&spi->report, IOWKIT100_IO_REPORT_SIZE);
    
    // OE High
    enable(false); // Disable Output

    spi->transfer16(pattern);  // Send the 16-bit pattern using SPI

    // Pulse LE HIGH after all bits are shifted in
    spi->report->Bytes[5] &= ~0x70; // D14B->Q1 High; STP_LE High
    IowKitWrite(*spi->DevHandle, IOW_PIPE_IO_PINS, (PCHAR)&spi->report, IOWKIT100_IO_REPORT_SIZE);
    std::this_thread::sleep_for(std::chrono::microseconds(10));

    spi->report->Bytes[5] |= 0x70; // D14B->Q1 Low; STP_LE Low
    IowKitWrite(*spi->DevHandle, IOW_PIPE_IO_PINS, (PCHAR)&spi->report, IOWKIT100_IO_REPORT_SIZE);

	// OE Low
    enable(true); // Enable Output
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
