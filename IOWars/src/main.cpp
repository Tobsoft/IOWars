// main.cpp

#include "IOWarsWeiT.hpp"

int main() {
    IOWars ioWars;
    system("color 0A");
    bool dir = 1;
    uint16_t pattern = 0x1331;
    while (!_kbhit()) {
        // Rotating pattern over 16 bit
		ioWars.stargate.ledbar.setAll(pattern);
        if (dir) {
            pattern = (pattern << 1) | ((pattern & 0x8000) / 0x8000);
        }
        else {
            pattern = (pattern >> 1) | ((pattern & 0x0001) * 0x8000);
        }
        Sleep(250);
    }
    _getch();
    ioWars.stargate.ledbar.clear();
    system("pause");

    return 0;
}

/*
int main() {
    IOWars ioWars;
    ioWars.stargate.off();
    ioWars.lcd.write("IOWars  | WeiT\nHTL Sbg | 3AHEL");
    system("pause");
    ioWars.stargate.ledbar.setAll(0x5555);
    ioWars.stargate.enable_backlight(true);

    printf("To test 7-Segment-Display, ");
	system("pause");
    //ioWars.stargate.seg7.writeString("08.1.1");
    //system("pause");
    ioWars.stargate.seg7.stopDisplay();
    system("pause");
	ioWars.stargate.seg7.count();

    printf("To test I2C communication, ");
    system("pause");
    dateTime time;
    //ioWars.rtc.getDateTime(time);
    //printf("Time: %d:%d:%d", time.hours, time.minutes, time.seconds);

    printf("To Control RGB LEDs: ");
    system("pause");
    for (int i = 0; i < 12; i++) {
        if (i%2 == 0) {
            ioWars.stargate.rgb.setLEDColor(i, 0, 255, 0);
        }
        else
            ioWars.stargate.rgb.setLEDColor(i, 0, 0, 255);
        }
    ioWars.stargate.rgb.updateLEDs();
    system("pause");
    
    ioWars.leds.loopAll();

    printf("To control Sound, ");
    system("pause");
    // Run sound playback in a separate thread
    std::thread soundThread([&]() {
        ioWars.stargate.sound.playMidi("midi/Improv.mid");
        printf("Midi played!\n");
    });

    ioWars.lcd.write("Hello, World!");

    // Ensure the sound thread finishes before exiting
    if (soundThread.joinable()) {
        soundThread.join();
    }
    printf("To exit, ");
    system("pause");
    ioWars.stargate.off();

    return 0;
}
*/