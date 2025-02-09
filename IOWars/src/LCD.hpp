// LCD.hpp

#ifndef LCD_H
#define LCD_H

class LCD {
	public:
		LCD(IOWKIT_HANDLE *deviceHandle, IOWKIT100_IO_REPORT *report);
		~LCD();
		void write(const char* message);
		void init();
		void clear();
		
	private:
		IOWKIT_HANDLE *DevHandle;         // Reference to IOWarrior handle
    	IOWKIT100_IO_REPORT *report;      // Shared report reference
		ULONG savedReport[12];
    	void setOn(bool on);
    	void setRS(bool state);
    	void setRW(bool read);
    	void setEN(bool state);
    	void setBacklight(bool state);
    	void setData(byte data);
    	bool isBusy();
    	void sendInstruction();
		void writeChar(char c);
		void setLine(int line);
};

#endif
