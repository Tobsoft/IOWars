// I2C.cpp

#include "IOWarsWeiT.hpp"
#include "I2C.hpp"

I2C::I2C(IOWKIT_HANDLE* deviceHandle, int speed) : DevHandle(deviceHandle) {
    IowKitSetTimeout(*DevHandle, 500);
    enable(true, speed);
}

I2C::~I2C() {

}

void I2C::enable(bool state, int speed) {
	IOWKIT100_SPECIAL_REPORT enableReport;
	enableReport.ReportID = 0x01;
	enableReport.Bytes[0] = state;
	enableReport.Bytes[1] = speed; // Communication Speed for I2C
	IowKitWrite(*DevHandle, IOW_PIPE_I2C_MODE, (PCHAR)&enableReport, IOWKIT100_I2C_REPORT_SIZE);
}

void I2C::write(byte* data) {
    int length = sizeof(*data);
    const int MAX_DATA_BYTES = 62;
    IOWKIT100_SPECIAL_REPORT report;
    memset(&report, 0x00, sizeof(report));

    report.ReportID = 0x02; // Report ID for write transactions

    int bytesSent = 0;
    while (bytesSent < length) {
        int chunkSize = (length - bytesSent > MAX_DATA_BYTES) ? MAX_DATA_BYTES : (length - bytesSent);

        // Set flags
        report.Bytes[0] = 0;
        if (bytesSent == 0) {
            report.Bytes[0] |= (1 << 7); // "Generate Start"
        }
        if (bytesSent + chunkSize >= length) {
            report.Bytes[0] |= (1 << 6); // "Generate Stop"
        }
        report.Bytes[0] |= chunkSize; // Set data count

        // Copy data correctly
        memcpy(&report.Bytes[1], &data[bytesSent], chunkSize);

        // Send report and check return value
        int writtenBytes = IowKitWrite(*DevHandle, IOW_PIPE_I2C_MODE, (PCHAR)&report, sizeof(report));
        if (writtenBytes != sizeof(report)) {
            printf("I2C Write Error: Failed to write data!\n");
            return;
        }

        bytesSent += chunkSize;

        // Read acknowledgment
        IOWKIT100_SPECIAL_REPORT ackReport;
        memset(&ackReport, 0x00, sizeof(ackReport));

        int bytesRead = IowKitRead(*DevHandle, IOW_PIPE_I2C_MODE, (PCHAR)&ackReport, IOWKIT100_I2C_REPORT_SIZE);
        if (bytesRead <= 0) {
            printf("I2C Read Error: No acknowledgment received!\n");
            return;
        }

        // Check error flags
        if (ackReport.Bytes[0] & 0x80) {
            printf("I2C Error detected!\n");
        }
        if (ackReport.Bytes[0] & 0x40) {
            printf("Lost arbitration!\n");
        }
        if ((ackReport.Bytes[0] & 0x3F) != chunkSize) {
            printf("Incorrect number of bytes returned in data count!\n");
        }

        // Handle specific errors
        switch (ackReport.Bytes[1]) {
        case I2C_NO_ERR:
            break;
        case I2C_ERR_WRONG_BYTE_COUNT:
            printf("Wrong number of bytes requested!\n");
            break;
        case I2C_ERR_NO_START:
            printf("Transaction without Start requested!\n");
            break;
        case I2C_ERR_NACK:
            printf("NACK received!\n");
            return;  // Exit if no acknowledgment
        case I2C_ERR_BUS:
            printf("Bus Error!\n");
            return;  // Exit if bus error occurs
        }
    }
}


void I2C::read(byte addr, byte* buffer, int length) {
    const int MAX_DATA_BYTES = 62;
    IOWKIT100_SPECIAL_REPORT report;
    memset(&report, 0, sizeof(report));

    report.ReportID = 0x03; // Report ID for read transactions
    report.Bytes[0] = length & 0xFF; // LSB of count
    report.Bytes[1] = addr; // Slave address
    report.Bytes[2] = (length >> 8) & 0xFF; // MSB of count

    // Send read request
    IowKitWrite(*DevHandle, IOW_PIPE_I2C_MODE, (PCHAR)&report, IOWKIT100_I2C_REPORT_SIZE);

    int bytesReceived = 0;
    while (bytesReceived < length) {
        IOWKIT100_SPECIAL_REPORT response;
        memset(&response, 0x00, IOWKIT100_I2C_REPORT_SIZE);

        // Read response
        int bytesRead = IowKitRead(*DevHandle, IOW_PIPE_I2C_MODE, (PCHAR)&response, IOWKIT100_I2C_REPORT_SIZE);

        if (bytesRead > 0 && response.ReportID == 0x03) {
            int chunkSize = response.Bytes[0] & 0x3F; // Extract data count
            memcpy(&buffer[bytesReceived], &response.Bytes[1], chunkSize);
            bytesReceived += chunkSize;
        }
    }
}
