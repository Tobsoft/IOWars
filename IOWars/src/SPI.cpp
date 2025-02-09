// SPI.cpp

#include "IOWarsWeiT.hpp"
#include "Stargate.hpp"
#include "SPI.hpp"

#include <iostream>
#include <thread>
#include <chrono>

SPI::SPI(IOWKIT_HANDLE* deviceHandle, IOWKIT100_IO_REPORT* report)
    : DevHandle(deviceHandle), report(report) {
    // Initialize pins to default states
    configurePins();
}

SPI::~SPI() {
    disableSPI();
}

void SPI::configurePins() {
    // Set SCK and MOSI as output, MISO as input, and SS as output
    report->Bytes[10] |= (1 << 0) | (1 << 1) | (1 << 2);  // Set SCK, MOSI, SS
    report->Bytes[11] &= ~(1 << 2);                      // Clear MISO as input

    // Write configuration
    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}

void SPI::disableSPI() {
    // Reset SCK and SS pins to low
    report->Bytes[10] &= ~((1 << 0) | (1 << 1));
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
    for (int i = 15; i >= 0; i--) {
        // Set MOSI bit
        if (data & (1 << i)) {
            report->Bytes[10] |= (1 << 2);  // MOSI high
        }
        else {
            report->Bytes[10] &= ~(1 << 2); // MOSI low
        }

        // Toggle clock SCK
        report->Bytes[10] |= (1 << 0);  // Clock high
        IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
        std::this_thread::sleep_for(std::chrono::microseconds(1));

        report->Bytes[10] &= ~(1 << 0); // Clock low
        IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    // Toggle !SS low to latch data
    report->Bytes[10] &= ~(1 << 1);  // SS low
    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
    std::this_thread::sleep_for(std::chrono::microseconds(1));

    // Bring !SS high to complete latch
    report->Bytes[10] |= (1 << 1);   // SS high
    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}

uint8_t SPI::readByte() {
    return readData;
}

