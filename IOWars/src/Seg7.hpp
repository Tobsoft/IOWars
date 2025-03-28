// Seg7.hpp

#ifndef SEG7_H
#define SEG7_H

#define NUM_DISPLAYS 4

#include <thread>
#include <atomic>
#include <string>

class Seg7 {
public:
	Seg7(IOWKIT_HANDLE* deviceHandle, IOWKIT100_IO_REPORT* report);
	~Seg7();
	void clear();
	void setDisplays(byte displays);
	void enable(bool state);
	void writeByte(byte b);
	void writeInt(int decimal, bool dot = false);
	void count();

	void writeString(const std::string& text);
	void stopDisplay();  // Function to stop multiplexing

private:
	IOWKIT_HANDLE* DevHandle;         // Reference to IOWarrior handle
	IOWKIT100_IO_REPORT* report;      // Shared report reference
	ULONG savedReport[12];

	unsigned char digitToSegment[10] = {
	0x3F, 0x06, 0x5B, 0x4F, 0x66,
	0x6D, 0x7D, 0x07, 0x7F, 0x6F
	};

	std::thread displayThread;
	std::atomic<bool> running;

	void multiplexDisplay(const std::string& text);
};

#endif