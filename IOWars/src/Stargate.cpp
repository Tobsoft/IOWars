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

void Stargate::off() {
    // Sound off
    sound.stop();

	// Backlight off
    enable_backlight(false);

    // I/O LEDs off
    setLeds(0x00);

	// RGB LEDs off
    rgb.enable(true);
    for (int i = 0; i < NUM_LEDS; i++) {
        rgb.setLEDColor(i, 0x00, 0x00, 0x00);
    }
	rgb.updateLEDs();
    rgb.enable(false);

    // Clear 7-Segment-Display
    seg7.enable(true);
    seg7.clear();
    seg7.enable(false);
}

void Stargate::enable_backlight(bool state) {
    // !LCD_ON must be high
    report->Bytes[4] |= (1 << 0);
    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);

    // SG-OUT must be on
    report->Bytes[5] |= 0x03; // set SG-OUT

    // STB must be on
    report->Bytes[0] |= (1 << 7);
    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);

    // Backlight is P8.6
    if (state) {
        report->Bytes[8] |= (1 << 6); // Set bit 6
    }
    else {
        report->Bytes[8] &= ~(1 << 6); // Clear bit 6
    }
    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}

/*
	Set the state of the Stargate LEDs (P8.0-P8.5)
	states: 0b 0 0 Re3 Re2 Re1 Li3 Li2 Li1
	Example: setLeds(0bXX000001) turns on LED Li1
	Example: setLeds(0bXX000011) turns on LED Li1 and LED Li2
	Example: setLeds(0bXX111111) turns all LEDs on
	Example: setLeds(0bXX000000) turns all LEDs off
*/
void Stargate::setLeds(byte states) {
    // 0b X X Re3 Re2 Re1 Li3 Li2 Li1
    // Stargate LEDs are P8.0 : P8.5

    report->Bytes[8] = (report->Bytes[8] & 0xC0) | (states & 0x3F);
    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);

    // 7SEG&!SPI should be low(SPI) to avoid 7Segment-write
    // For this, !LCD_ON (P4.0) must be high
    report->Bytes[4] |= (1 << 0); // !LCD_ON high
    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
    // SG_OUT must be on
    report->Bytes[5] |= 0x03; // SG-OUT
    //report->Bytes[5] &= ~(1 << 7); // 7SEG&!SPI low
    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}
