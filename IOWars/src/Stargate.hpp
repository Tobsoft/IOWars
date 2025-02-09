// Stargate.hpp

#ifndef STARGATE_H
#define STARGATE_H

#include "Seg7.hpp"
#include "RGB_LEDs.hpp"
#include "SPI.hpp"
#include "Ledbar.hpp"

class Stargate {
public:
	Stargate(IOWKIT_HANDLE* deviceHandle, IOWKIT100_IO_REPORT* report);
	~Stargate();

	Seg7 seg7;
	RGB_LEDs rgb;
	SPI spi;
	Ledbar ledbar;

	void enable_backlight(bool state);
	void setLeds(byte states); // 0b 0 0 Re3 Re2 Re1 Li3 Li2 Li1

	// TODO: Sound is P8.4

private:
	IOWKIT_HANDLE* DevHandle;         // Reference to IOWarrior handle
	IOWKIT100_IO_REPORT* report;      // Shared report reference
};

#endif
