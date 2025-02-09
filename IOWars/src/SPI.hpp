// SPI.hpp

#ifndef SPI_H
#define SPI_H

#include <cstdint>

class SPI {
public:
    SPI(IOWKIT_HANDLE* deviceHandle, IOWKIT100_IO_REPORT* report);
    ~SPI();

    void transferByte(uint8_t data);
    void transfer16(uint16_t data);
    uint8_t readByte();

private:
    IOWKIT_HANDLE* DevHandle;
    IOWKIT100_IO_REPORT* report;
    uint8_t readData = 0;

    void configurePins();
    void disableSPI();
};

#endif
