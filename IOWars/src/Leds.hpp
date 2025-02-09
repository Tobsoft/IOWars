// Leds.hpp

#ifndef LEDS_H
#define LEDS_H

class Leds {
	public:
		Leds(IOWKIT_HANDLE *deviceHandle, IOWKIT100_IO_REPORT *report);
		~Leds();
		void clear();
		void allOn();
		void controlLeds(ULONG PA, ULONG PB, ULONG PC, ULONG PD);
		void loopAll();
		void byteToPorts(ULONG byte, ULONG *PA, ULONG *PB, ULONG *PC, ULONG *PD);
		int delay_ms;
		ULONG PA, PB, PC, PD;
		
	private:
		IOWKIT_HANDLE *DevHandle;         // Reference to IOWarrior handle
    	IOWKIT100_IO_REPORT *report;      // Shared report reference
    	
    	void init();
};

#endif
