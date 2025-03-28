// SPI.cpp

#include "IOWarsWeiT.hpp"
#include "Stargate.hpp"
#include "SPI.hpp"

#include <iostream>
#include <thread>
#include <chrono>

SPI::SPI(IOWKIT_HANDLE* deviceHandle, IOWKIT100_IO_REPORT* report)
    : DevHandle(deviceHandle), report(report) {
}

SPI::~SPI() {

}

void SPI::enableSPI() {
    // Set !LCD_ON high
    report->Bytes[4] |= (1 << 0);

    // Set 7SEG&!SPI (P5.7) to Low
    report->Bytes[5] &= ~(1 << 7);

    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}

void SPI::disableSPI() {
    report->Bytes[4] |= (1 << 0); // !LCD_ON high
    report->Bytes[5] |= (1 << 7); // 7SEG&!SPI high

    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}

void SPI::transferByte(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        // Set MOSI bit based on data MSB
        if (data & 0x80) {
            report->Bytes[10] |= (1 << 2);  // MOSI high
        }
        else {
            report->Bytes[10] &= ~(1 << 2); // MOSI low
        }

        // Toggle SCK (rising edge)
        report->Bytes[10] |= (1 << 0);
        IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
        std::this_thread::sleep_for(std::chrono::microseconds(10)); // Clock high period

        // Read MISO on falling edge
        uint8_t bit = (report->Bytes[11] & (1 << 2)) ? 1 : 0;
        readData = (readData << 1) | bit;

        // Set SCK low (falling edge)
        report->Bytes[10] &= ~(1 << 0);
        IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
        std::this_thread::sleep_for(std::chrono::microseconds(10)); // Clock low period

        data <<= 1; // Shift data to next bit
    }
}

void SPI::transfer16(uint16_t data) {
    enableSPI();

    report->Bytes[10] &= ~(1 << 0); // SCK LOW
    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);

    // Shift out 16 bits MSB first
    for (int i = 15; i >= 0; i--) {
        // Set MOSI bit (data is stable before the clock rising edge)
        if (data & (1 << i)) {
            report->Bytes[10] |= (1 << 2);  // MOSI HIGH
        }
        else {
            report->Bytes[10] &= ~(1 << 2); // MOSI LOW
        }

        IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);

        // Toggle clock (SCK)
        report->Bytes[10] |= (1 << 0);  // SCK HIGH (rising edge)
        IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
        std::this_thread::sleep_for(std::chrono::microseconds(1));

        report->Bytes[10] &= ~(1 << 0); // SCK LOW (falling edge)
        IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
}


uint8_t SPI::readByte() {
    return readData;
}

