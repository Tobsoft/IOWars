// Seg7.cpp

#include "IOWarsWeiT.hpp"

// Constructor with references to DevHandle and report
Seg7::Seg7(IOWKIT_HANDLE* deviceHandle, IOWKIT100_IO_REPORT* reportBuffer)
	: DevHandle(deviceHandle), report(reportBuffer), savedReport()
{

}

Seg7::~Seg7() {

}

void Seg7::setDisplays(byte displays) {
	// Ensure LE of Ledbar is Low, so no unwanted data is shifted (SCK of SPI is P10.0)
	report->Bytes[5] &= 0b11001111; // P5.5 + P5.4 Low (1oo4 logic)

	enable(true);
	// Display Control = P10.3:P10.0
	displays &= 0x0F; // Ensure displays is only using the low nibble

	// Clear the low nibble of report->Bytes[10] and set it to displays
	report->Bytes[10] = (report->Bytes[10] & 0xF0) | displays;

	IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}

void Seg7::enable(bool state) {
	// SG-OUT is not needed: Set it off so 7Seg does not interfere with backlights etc.
	report->Bytes[5] &= 0xFC;
	IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE); 

	// 7SEG & !SPI = P5.7
	report->Bytes[5] |= 0x80;
	IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}

void Seg7::writeByte(byte b) {
	enable(true);
	report->Bytes[8] = b;
	IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}

void Seg7::writeInt(int decimal, bool dot) {
	// Validate input: Ensure the decimal is between 0 and 9
	if (decimal < 0 || decimal > 9) {
		return;
	}

	byte value = digitToSegment[decimal] | (dot << 7);

	// Write the corresponding segment byte to the display
	writeByte(value);
}

void Seg7::count() {
	printf("7Seg Count\n");
	setDisplays(0x0F);
	int i = 0;
	while (!_kbhit()) {
		writeInt(i);
		i++;
		Sleep(500);
		if (i >= 10) {
			i = 0;
		}
	}
	int ignored = _getch();
	clear();
}

void Seg7::clear() {
	writeByte(0x00);
}

void Seg7::writeString(const std::string& text) {
	stopDisplay();  // Stop any previous display thread

	running = true;
	displayThread = std::thread(&Seg7::multiplexDisplay, this, text);  // Start new thread
}

void Seg7::stopDisplay() {
	running = false;
	if (displayThread.joinable()) {
		displayThread.join();  // Ensure thread is safely stopped
	}
}

void Seg7::multiplexDisplay(const std::string& text) {
	const int delayMs = 0;
	int currentDisplay = 0;

	while (running) {
		currentDisplay = 0;  // Reset display position
		for (size_t i = 0; i < text.length(); i++) {
			if (!running) return;  // Stop safely if needed

			if (currentDisplay >= NUM_DISPLAYS) break;  // Limit to 4 displays

			char c = text[i];
			bool dot = false;  // Default: No dot

			// Check for dot AFTER a digit (e.g., "1.1")
			if (c >= '0' && c <= '9') {
				if (i < text.length() - 1 && text[i + 1] == '.') {
					dot = true;
					i++;  // Skip dot in processing
				}

				// Activate correct display
				setDisplays(1 << currentDisplay);  // Use bit shift for correct activation
				writeInt(c - '0', dot);

				currentDisplay++;  // Only increment when a digit is set
			}
			else {
				writeByte(0x00);  // Ignore non-numeric characters
			}

			Sleep(delayMs);  // Short delay to prevent flickering
		}
	}

	clear();  // Ensure display is cleared when stopping
}
