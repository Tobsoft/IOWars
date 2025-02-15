#ifndef HELPER_H
#define HELPER_H

#include <cstdio>
#include <cstdint>

// Print a 32-bit unsigned integer in binary form
void printBinary(uint32_t value) {
    for (int i = 31; i >= 0; i--) { // Loop from the most significant bit (31) to the least significant bit (0)
        printf("%d", (value >> i) & 1); // Extract and print each bit
    }
    printf("\n");
}

// Print the contents of a report, including ReportID, Bytes, and optional report type
void printReport(const IOWKIT100_SPECIAL_REPORT& report, const char* reportType = "Special Report") {
    printf("=== Report (%s) ===\n", reportType);
    printf("ReportID: 0x%02X\n", report.ReportID);
    printf("Bytes: ");
    for (size_t i = 0; i < sizeof(report.Bytes); i++) {
        printf("%02X ", report.Bytes[i]);
    }
    printf("\n");
}

#endif // HELPER_H
