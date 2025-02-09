// RGB_LEDs.cpp

#include "IOWarsWeiT.hpp"

// Constructor initializes LED buffer to off (black)
RGB_LEDs::RGB_LEDs(IOWKIT_HANDLE* deviceHandle, IOWKIT100_IO_REPORT* reportBuffer)
    : DevHandle(deviceHandle), report(reportBuffer) {
    // Clear LED buffer (set all LEDs to black)
    std::memset(ledBuffer, 0, sizeof(ledBuffer));
    // Enable digital LED function in special mode
    enable(true);
}

RGB_LEDs::~RGB_LEDs() {
    // Turn off LEDs upon destruction
    std::memset(ledBuffer, 0, sizeof(ledBuffer));
    updateLEDs();
}

void RGB_LEDs::enable(bool state) {
    if (state) {
        LED_report.ReportID = 0x28;
        LED_report.Bytes[0] = 0x01;  // Enable digital LED mode
        LED_report.Bytes[1] = _0CYC; // Timing setup
        LED_report.Bytes[2] = _0PHA;
        LED_report.Bytes[3] = _1CYC;
        LED_report.Bytes[4] = _1PHA;
        IowKitWrite(*DevHandle, IOW_PIPE_SPECIAL_MODE, (PCHAR)&LED_report, IOWKIT100_SPECIAL_REPORT_SIZE);
    }
    else {
        LED_report.ReportID = 0x28;
        LED_report.Bytes[0] = 0x00;  // Disable digital LED mode
        LED_report.Bytes[1] = _0CYC; // Timing setup
        LED_report.Bytes[2] = _0PHA;
        LED_report.Bytes[3] = _1CYC;
        LED_report.Bytes[4] = _1PHA;
        IowKitWrite(*DevHandle, IOW_PIPE_SPECIAL_MODE, (PCHAR)&LED_report, IOWKIT100_SPECIAL_REPORT_SIZE);
    }
}

// Set color for a specific LED
void RGB_LEDs::setLEDColor(int ledIndex, uint8_t red, uint8_t green, uint8_t blue) {
    if (ledIndex < 0 || ledIndex >= NUM_LEDS) {
        throw std::out_of_range("LED index out of range");
    }

    int offset = ledIndex * 3;
    ledBuffer[offset] = green;  // WS2812 expects GRB order
    ledBuffer[offset + 1] = red;
    ledBuffer[offset + 2] = blue;
}

void RGB_LEDs::updateLEDs() {
    // Enable digital LED function in special mode
    enable(true);

    // Calculate number of blocks and LED counts
    const int MAX_LEDS_PER_BLOCK = 20;
    int totalBlocks = (NUM_LEDS + MAX_LEDS_PER_BLOCK - 1) / MAX_LEDS_PER_BLOCK;  // Ceiling division
    int remainingLEDs = NUM_LEDS;

    for (int blockIndex = 0; blockIndex < totalBlocks; ++blockIndex) {
        LED_report.ReportID = 0x29;
        LED_report.Bytes[0] = blockIndex;  // Block index (0 - 24)

        // Handle LED count per block
        int currentBlockLEDs = remainingLEDs >= MAX_LEDS_PER_BLOCK ? MAX_LEDS_PER_BLOCK : remainingLEDs;
        remainingLEDs -= currentBlockLEDs;

        LED_report.Bytes[1] = currentBlockLEDs & 0xFF;          // Number of LEDs LSB
        LED_report.Bytes[2] = (currentBlockLEDs >> 8) & 0xFF;   // Number of LEDs MSB

        // Fill color data for the current block
        int bufferOffset = blockIndex * MAX_LEDS_PER_BLOCK * 3;  // Each LED uses 3 bytes (GRB)
        for (int i = 0; i < currentBlockLEDs * 3; ++i) {
            LED_report.Bytes[3 + i] = ledBuffer[bufferOffset + i];
        }

        // Set high bit to indicate data should be sent to LEDs
        LED_report.Bytes[0] |= 0x80;

        // Write block to LEDs
        IowKitWrite(*DevHandle, IOW_PIPE_SPECIAL_MODE, (PCHAR)&LED_report, IOWKIT100_SPECIAL_REPORT_SIZE);
    }
}
