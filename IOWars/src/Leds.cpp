// Leds.cpp

#include "IOWarsWeiT.hpp"
#include "LedPatterns.hpp"

// Constructor with references to DevHandle and report
Leds::Leds(IOWKIT_HANDLE *deviceHandle, IOWKIT100_IO_REPORT *reportBuffer)
    : DevHandle(deviceHandle), report(reportBuffer)
{
	delay_ms = 20;
	init();
}

Leds::~Leds() {
	
}

void Leds::init() {
	report->ReportID=0x00;   //82bit für 82pin Modus
	report->Bytes[0]=0x80; 	//Strobe (STB) Signal auf HIGH (Leitung P0->7)
	report->Bytes[1]=0xFF;	// Port 1 ist unbenutzt (siehe Blockschaltbild, SLP)
	report->Bytes[2]=0xFF;	// I2C; aktuell unbenutzt, daher 0xFF
	report->Bytes[3]=0xFF;	// Port 3 ist unbenutzt (siehe Blockschaltbild, SLP)
	report->Bytes[4]=0xFF;	// LCD; aktuell unbenutzt, daher 0xFF
	report->Bytes[5]=0x05;	// Steuerschaltung ?ber D10 + LCD; aktuell unbenutzt, daher 0xFF; PC und PD mit 0x0A - PA und PB mit 0x05
	report->Bytes[6]=0xFF;	// Port 6 ist unbenutzt (siehe Blockschaltbild, SLP)				
	report->Bytes[7]=0xFF;	// Port 7 ist unbenutzt (siehe Blockschaltbild, SLP)				
	report->Bytes[8]=PA;		// LED-Ports PA (PC und Stargate-LEDs)
	report->Bytes[9]=PB;		// LED-Ports PB	(PD und Tastenabfrage vom Stargate)
	report->Bytes[10]=0xFF;	// SPI; aktuell unbenutzt, daher 0xFF
	report->Bytes[11]=0xFF;	// SPI; aktuell unbenutzt, daher 0xFF
}

void Leds::clear() {
	report->Bytes[5] = 0x05; // PA & PB
	report->Bytes[8] = 0x00; // PA/PC
	report->Bytes[9] = 0x00; // PB/PD
	IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
	report->Bytes[5] = 0x0A; // PC & PD
	IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}

void Leds::allOn() {
	report->Bytes[5] = 0x05; // PA & PB
	report->Bytes[8] = 0xFF; // PA/PC
	report->Bytes[9] = 0xFF; // PB/PD
	IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
	report->Bytes[5] = 0x0A; // PC & PD
	IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}

void Leds::byteToPorts(ULONG byte, ULONG *PA, ULONG *PB, ULONG *PC, ULONG *PD) {
    *PA = byte & 0xFF;          // Lowest 8 bits
    *PB = (byte >> 8) & 0xFF;   // Next 8 bits
    *PC = (byte >> 16) & 0xFF;  // Next 8 bits
    *PD = (byte >> 24) & 0xFF;  // Highest 8 bits
}

void Leds::controlLeds(ULONG PA, ULONG PB, ULONG PC, ULONG PD) {
	report->Bytes[5] = 0x05; // PA & PB
	report->Bytes[8] = PA; // PA/PC
	report->Bytes[9] = PB; // PB/PD
	IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
	report->Bytes[8] = PC; // PA/PC
	report->Bytes[9] = PD; // PB/PD
	report->Bytes[5] = 0x0A; // PC & PD
	IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}

void Leds::loopAll() {
    clear();
    system("pause");
    allOn();
    system("pause");
    controlLeds(0x55, 0x55, 0x55, 0x55);
    system("pause");

    pattern1(*this);
    pattern2(*this);
    pattern3(*this);
    pattern4(*this);
    patternClock(*this);

    clear();
}


