// Stargate.cpp

#include "IOWarsWeiT.hpp"
#include "Stargate.hpp"

// Konstruktor mit Initialisierungsliste
Stargate::Stargate(IOWKIT_HANDLE* deviceHandle, IOWKIT100_IO_REPORT* report)
    : DevHandle(deviceHandle), report(report), seg7(deviceHandle, report), rgb(deviceHandle, report), spi(deviceHandle, report), ledbar(&spi), sound(deviceHandle)
{

}

Stargate::~Stargate() {

}

void Stargate::enable_backlight(bool state) {
    // Backlight is P8.6
    // PC must be on
    report->Bytes[5] = 0x0A; // PC & PD
    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
    if (state) {
        report->Bytes[8] |= (1 << 6); // Set bit 6
    }
    else {
        report->Bytes[8] &= ~(1 << 6); // Clear bit 6
    }
    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}

void Stargate::setLeds(byte states) {
    // 0b 0 0 Re3 Re2 Re1 Li3 Li2 Li1
    // Stargate LEDs are P8.0 : P8.5
    // SG_OUT must be on
    report->Bytes[5] = 0x0F; // SG-OUT & SG-IN
    report->Bytes[8] = (report->Bytes[8] & 0xC0) | (states & 0x3F);
    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}
