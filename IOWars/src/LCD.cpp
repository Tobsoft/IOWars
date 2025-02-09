// LCD.cpp

#include "IOWarsWeiT.hpp"

// Constructor with references to DevHandle and report
LCD::LCD(IOWKIT_HANDLE *deviceHandle, IOWKIT100_IO_REPORT *reportBuffer)
    : DevHandle(deviceHandle), report(reportBuffer) 
{

}

LCD::~LCD() {
	
}

void LCD::setOn(bool on) {
	// !on = P4.0
	// enables LCD if on == true
    if (!on) {
        report->Bytes[4] |= (1); // Set bit 0, disable LCD
    } else {
        report->Bytes[4] &= ~(1); // Clear bit 0, enable LCD
    }
    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}

void LCD::setRS(bool state) {
	// RS = P4.1
    if (state) {
        report->Bytes[4] |= (1 << 1); // Set bit 1
    } else {
        report->Bytes[4] &= ~(1 << 1); // Clear bit 1
    }
    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}

void LCD::setRW(bool read) {
	// R/!W = P4.2
    if (read) {
        report->Bytes[4] |= (1 << 2); // Set bit 2
    } else {
        report->Bytes[4] &= ~(1 << 2); // Clear bit 2
    }
    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}

void LCD::setEN(bool state) {
	// EN = P4.3
	if (state) {
        report->Bytes[4] |= (1 << 3); // Set bit 3
    } else {
        report->Bytes[4] &= ~(1 << 3); // Clear bit 3
    }
    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}

void LCD::setBacklight(bool state) {
	// LCD_LK = P8.7
	if (state) {
		report->Bytes[8] |= (1 << 7); // Set bit 7
	} else {
		report->Bytes[8] &= ~(1 << 7); // Clear bit 7
	}
    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}

void LCD::setData(byte data) {
	report->Bytes[5] = data;
    IowKitWrite(*DevHandle, IOW_PIPE_IO_PINS, (PCHAR)report, IOWKIT100_IO_REPORT_SIZE);
}

bool LCD::isBusy() {
	ULONG data;
	IOWKIT100_IO_REPORT readBuffer;
    
    // Save current state of report.Bytes
	memcpy(savedReport, report->Bytes, sizeof(report->Bytes));

    // Prepare control signals for reading
    setRS(false); // Instruction register
    setRW(true);  // Read mode
    setEN(true);

    // Read data from the LCD
    data = IowKitRead(DevHandle, IOW_PIPE_IO_PINS, (PCHAR)&readBuffer, sizeof(readBuffer));

    // Debug: Print read data
    /*
    printf("Read data: ");
    printBinary(data);
    printf("\n");
    */

    setEN(false); // End read operation

    // Restore original state of report.Bytes
    memcpy(report->Bytes, savedReport, sizeof(report->Bytes));
	//IowKitWrite(DevHandle, IOW_PIPE_IO_PINS, (PCHAR)&report, IOWKIT100_IO_REPORT_SIZE);

    return data & 0x80; // Return busy flag state
}

void LCD::sendInstruction() {
	while (isBusy()) {
        // Wait until the LCD is ready
    }
    setEN(true);
    setEN(false);
}

void LCD::clear() {
	setRS(false);
	setRW(false);
	setData(0x01);
	sendInstruction();
	Sleep(2); // Wait > 1.52ms
}

void LCD::init() {
    // Enable LCD
    setOn(true);
    // Set Backlight
    setBacklight(true);

    // Initialize the LCD in 8-bit mode
    setRS(false);
    setRW(false);
    setData(0x38); // Function set: 8-bit, 2-line, 5x8 dots
    setEN(true);
    setEN(false);

    Sleep(5); // Wait > 4.1ms
    setEN(true);
    setEN(false);

    Sleep(1); // Wait > 100us
    setEN(true);
    Sleep(1); // Wait
    setEN(false);

    // Set display control: display ON, cursor OFF, no blink
    setData(0x0C);
    setEN(true);
    setEN(false);

    // Set entry mode: increment automatically, no shift
    setData(0x06);
    setEN(true);
    setEN(false);

    // Clear display
    clear();

    printf("LCD Initialized!\n");
}

void LCD::setLine(int line) {
    // Validate the input line number
    if (line != 1 && line != 2) {
        printf("Invalid line number. Please use 1 for the first line or 2 for the second line.\n");
        return;
    }

    // Set the DDRAM address for the selected line
    ULONG ddram_address = (line == 1) ? 0x00 : 0x40;

    // Prepare to send the instruction to set the cursor to the start of the selected line
    setRS(false);       // Instruction register
    setRW(false);       // Write mode
    setData(0x80 | ddram_address); // Command to set DDRAM address (0x80 is the base command)
    sendInstruction();  // Send the instruction
}

void LCD::writeChar(char c) {
    setRS(true);  // Select data register
    setRW(false); // Write mode
    setData(c);   // Write character data
    sendInstruction();
}

void LCD::write(const char *message) {
	init();
	// Define the maximum characters per line for a 16x2 LCD
    size_t max_length_per_line = 16;

    size_t line = 1; // Start writing on the first line
    size_t char_count = 0; // Count characters on the current line

    setLine(line); // Set to the first line initially

    for (size_t i = 0; message[i] != '\0'; ++i) {
        if (message[i] == '\n') {
            // Move to the next line if a newline character is encountered
            if (line == 1) {
                line = 2;
                setLine(line);
                char_count = 0; // Reset the character count for the new line
            } else {
                // Ignore additional lines if there are more than two
                break;
            }
        } else {
            if (char_count < max_length_per_line) {
                // Write the character if within the max length of the line
                writeChar(message[i]);
                char_count++;
            }
            // If max_length_per_line is exceeded, go to the next line
            else {
            	line = 2;
                setLine(line);
                // Write the character if within the max length of the line
                writeChar(message[i]);
                char_count++;
                char_count = 0; // Reset the character count for the new line
			}
        }
    }
    printf("\nWrote String to LCD:\n%s", message);
    printf("\n");
}
