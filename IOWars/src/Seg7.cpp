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
	// Display Control = P10.3:P10.0
	displays &= 0x0F;
	memcpy(savedReport, report->Bytes, sizeof(report->Bytes));
	report->Bytes[10] = displays;
	IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
	memcpy(report->Bytes, savedReport, sizeof(report->Bytes));
}

void Seg7::enable(bool state) {
	// 7SEG & !SPI = P5.7
	report->Bytes[5] = 0x80;
	if (state) {
		report->Bytes[8] |= (1 << 7); // Set bit 7
	}
	else {
		report->Bytes[8] &= ~(1 << 7); // Clear bit 7
	}
	IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}

void Seg7::writeByte(byte b) {
	enable(true);
	report->Bytes[8] = b;
	IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}

void Seg7::writeInt(int decimal) {
	// Validate input: Ensure the decimal is between 0 and 9
	if (decimal < 0 || decimal > 9) {
		return;
	}

	// Write the corresponding segment byte to the display
	writeByte(digitToSegment[decimal]);
}

void Seg7::count() {
	printf("7Seg Count\n");
	int i = 0;
	while (!_kbhit()) {
		writeInt(i);
		i++;
		Sleep(500);
		if (i >= 10) {
			i = 0;
		}
	}
	_getch();
	writeByte(0x00);
}

void Seg7::clear() {
	writeByte(0x00);
}