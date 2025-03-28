// SPI.hpp

#ifndef SPI_H
#define SPI_H

#include <cstdint>

class SPI {
public:
    IOWKIT_HANDLE* DevHandle;
    IOWKIT100_IO_REPORT* report;
    SPI(IOWKIT_HANDLE* deviceHandle, IOWKIT100_IO_REPORT* report);
    ~SPI();

    void transferByte(uint8_t data);
    void transfer16(uint16_t data);

    void enableSPI();
    void disableSPI();
    uint8_t readByte();

private:
    uint8_t readData = 0;
};

#endif
