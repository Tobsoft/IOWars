// IOWarsWeiT.hpp

#ifndef IOWARS_H
#define IOWARS_H

#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <time.h>

extern "C" {
#include<iowkit.h>
};

#include "LCD.hpp"
#include "Leds.hpp"
#include "Stargate.hpp"
#include "I2C.hpp"
#include "DS1307.hpp"

class IOWars {
	public:
		// Constructor and Destructor
	    IOWars();                   // Default Constructor
	    ~IOWars();                  // Destructor to handle cleanup
	    
	    // Hardware Elements
	    LCD lcd;
	    Leds leds;
		Stargate stargate;
		I2C i2c;
		DS1307 rtc;
	    
	private:
		IOWKIT_HANDLE DevHandle;									//Variable f. Geräte(IOW)Nummer
		ULONG ComResult;
		
		IOWKIT100_IO_REPORT report;
		ULONG savedReport[12];
		IOWKIT100_IO_REPORT readBuffer;
		
		// Initialization
	    bool initializeDevice();
};

#endif
