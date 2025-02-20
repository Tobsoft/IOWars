// IOWarsWeiT.cpp

#include "IOWarsWeiT.hpp"

// Constructor
IOWars::IOWars() 
    : lcd(&DevHandle, &report), leds(&DevHandle, &report), stargate(&DevHandle, &report), i2c(&DevHandle), rtc(i2c)
{
	SetConsoleOutputCP(1252);
    if (!initializeDevice()) {
        exit(EXIT_FAILURE);
    }
}


// Destructor
IOWars::~IOWars() {
    if (DevHandle) {
        IowKitCloseDevice(DevHandle);
    }
}

// Initialize IOWarrior Device
bool IOWars::initializeDevice() {
    DevHandle = IowKitOpenDevice();
    
    while (!DevHandle) {
        printf("Failed to initialize IOWarrior device.\nPress [Enter] to retry or press [ESC] to exit\n");

        // Wait for user input
        int c = _getch();

        // Handle the user input
        if (c == 27 || c == 13) {
        	system("cls"); // Clear Screen
        	if (c == 27) {  // ASCII for ESC key
	            printf("Exiting initialization.\n");
	            return false;
	        }
	        else if (c == 13) {  // ASCII for Enter key
	            printf("Retrying...\n");
	            DevHandle = IowKitOpenDevice();  // Retry initialization
	        }
		}
    }

    printf("IOWarrior device initialized successfully.\n");
    return true;
}
