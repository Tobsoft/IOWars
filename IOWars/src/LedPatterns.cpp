
#include "IOWarsWeiT.hpp"
#include "LedPatterns.hpp"
#include <conio.h>  // For _kbhit(), _getch()
#include <ctime>
#include <windows.h> // For Sleep()

void pattern1(Leds &leds) {
    printf("Led Pattern 1\n");
    int i = 0;
    do {
        ULONG PA, PB, PC, PD;
        leds.byteToPorts(1 << i, &PA, &PB, &PC, &PD);
        leds.controlLeds(PA, PB, PC, PD);
        i = (i + 1) % 32;
        Sleep(leds.delay_ms);
    } while (!_kbhit());
    _getch();
    leds.clear();
}

void pattern2(Leds &leds) {
    printf("Led Pattern 2\n");
    int i = 0;
    do {
        ULONG PA, PB, PC, PD;
        ULONG byte = (1 << i) | (1 << ((i + 16) % 32));
        leds.byteToPorts(byte, &PA, &PB, &PC, &PD);
        leds.controlLeds(PA, PB, PC, PD);
        i = (i + 1) % 32;
        Sleep(leds.delay_ms);
    } while (!_kbhit());
    _getch();
    leds.clear();
}

void pattern3(Leds &leds) {
    printf("Led Pattern 3\n");
    int i = 0; // Running index
    int j = 1; // Number of active LEDs
    do {
        ULONG byte = 0; // Initialize the byte to 0

        // Generate the bitmap for j active LEDs
        for (int k = 0; k < j; k++) {
            int position = (k * (32 / j) + i) % 32; // Evenly spaced positions, rotated by i
            byte |= (1 << position);               // Set the corresponding bit
        }

        leds.byteToPorts(byte, &leds.PA, &leds.PB, &leds.PC, &leds.PD);
        leds.controlLeds(leds.PA, leds.PB, leds.PC, leds.PD);

        i++; // Increment the index to rotate the pattern
        if (i >= 32) {
            i = 0;
            j++; // Increment the number of active LEDs
            if (j > 32) {
                j = 1; // Reset to 1 LED if j exceeds the total bit width
            }
        }
        Sleep(leds.delay_ms);
    } while (!_kbhit());
    _getch();
    leds.clear();
}

void pattern4(Leds &leds) {
	printf("Led Pattern 4\n");
    int i = 0;            // Running index
    int j = 2;           // Number of active LEDs
    int dir = 1;          // Direction: 1 for forward, -1 for backward
    int animDir = 1;

    do {
        ULONG byte = 0; // Initialize the byte to 0

        // Generate the bitmap for j active LEDs
        for (int k = 0; k < j; k++) {
            int position = (k * (32 / j) + i) % 32; // Evenly spaced positions, rotated by i
            byte |= (1 << position);               // Set the corresponding bit
        }

        leds.byteToPorts(byte, &leds.PA, &leds.PB, &leds.PC, &leds.PD);
        leds.controlLeds(leds.PA, leds.PB, leds.PC, leds.PD);

        i += dir; // Move in the current direction
        if (i >= 32) { // If we reach the end, reverse direction
            i = 31;  // Stay within bounds
            dir = -1; // Reverse direction to backward
        } else if (i < 0) { // If we reach the start, reverse direction
            i = 0;  // Stay within bounds
            dir = 1; // Reverse direction to forward

            // Reduce the number of LEDs when a full forward-backward cycle completes
            if (animDir == -1) {
                j /= 2; // Halve the number of LEDs
                if (j < 2) {
                	j = 4;
                	animDir = 1;
				}
            } else {
                j *= 2;
                if (j > 32) {
                	j = 16;
                	animDir = -1;
				}
            }
        }

        Sleep(leds.delay_ms);
    } while (!_kbhit());
    _getch();
    leds.clear();
}

void patternClock(Leds& leds) {
    printf("Clock Pattern\n");

    do {
        time_t now = time(NULL);
        struct tm current_time;

        // Use localtime_s instead of localtime
        if (localtime_s(&current_time, &now) != 0) {
            printf("Error retrieving local time.\n");
            break;
        }

        int hour = current_time.tm_hour;
        int minute = current_time.tm_min;
        int second = current_time.tm_sec;

        ULONG byte = 0;
        int hour_pos = ((hour * 32) / 12 - 1) % 32;
        int minute_pos = ((minute * 32) / 60 - 1) % 32;
        int second_pos = ((second * 32) / 60 - 1) % 32;

        byte |= (1 << hour_pos);
        byte |= (1 << minute_pos);
        byte |= (1 << second_pos);

        ULONG PA, PB, PC, PD;
        leds.byteToPorts(byte, &PA, &PB, &PC, &PD);
        leds.controlLeds(PA, PB, PC, PD);

        Sleep(10);
    } while (!_kbhit());

    _getch();
    leds.clear();
}

