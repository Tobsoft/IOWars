// main.cpp

#include "IOWarsWeiT.hpp"

#include <thread>
#include <atomic>

const char MIDI_FILE_PATH[] = "midi/Megalovania.mid"; // Path to the MIDI file

void stargateStroboscope(IOWars& ioWars, int delay_ms = 100) {
	ioWars.stargate.off();
	std::atomic<bool> stargateOn(true);

    std::thread stroboscopeThread([&]() {
        while (stargateOn) {
            for (int i = 0; i < NUM_LEDS; i++) {
                ioWars.stargate.rgb.setLEDColor(i, 255, 255, 255);
            }
            ioWars.stargate.rgb.updateLEDs();

            std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));

            for (int i = 0; i < NUM_LEDS; i++) {
                ioWars.stargate.rgb.setLEDColor(i, 0, 0, 0);
            }
            ioWars.stargate.rgb.updateLEDs();

            std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
        }
        });
    if (stroboscopeThread.joinable()) stroboscopeThread.join();
}

void stargateOpeningAnimation(IOWars& ioWars) {
    ioWars.stargate.off();
    std::atomic<bool> soundPlaying(true);

    // Start playing the MIDI file in a separate thread
    std::thread soundThread([&]() {
        ioWars.stargate.sound.playMidi(MIDI_FILE_PATH);
        soundPlaying = false; // Indicate when the sound has finished
        });

    // RGB LEDs animation with random mode switching
    std::thread rgbThread([&]() {
        int mode = rand() % 3; // Randomly select between 3 modes
        int hue = 0;
        int currentLED = 0;
        int iteration = 0;
        int secondsPerMode = 5;
        auto lastModeChange = std::chrono::steady_clock::now();
        while (soundPlaying) {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - lastModeChange).count() >= secondsPerMode) {
                mode = rand() % 3; // Randomly select between 3 modes
                lastModeChange = now;
            }

            switch (mode) {
            case 0: // Running white LED
                for (int i = 0; i <= currentLED; ++i) {
                    ioWars.stargate.rgb.setLEDColor(i, 255, 255, 255);
                    ioWars.stargate.rgb.updateLEDs();
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                    ioWars.stargate.rgb.setLEDColor(i, 0, 0, 255); // Set to blue
                    ioWars.stargate.rgb.updateLEDs();
                }
                currentLED = (currentLED + 1) % 12;
                break;

            case 1: // Alternating blinking blue effect
                for (int i = 0; i < NUM_LEDS; i++) {
                    if (i % 2 == iteration % 2) {
                        ioWars.stargate.rgb.setLEDColor(i, 0, 0, 255);
                    }
                    else {
                        ioWars.stargate.rgb.setLEDColor(i, 0, 0, 0);
                    }
                }
                ioWars.stargate.rgb.updateLEDs();
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
				iteration++;
                break;

            case 2: // Rotating color gradient
                for (int i = 0; i < 12; i++) {
                    int adjustedHue = (hue + (i * 30)) % 360;
                    int red, green, blue;
                    if (adjustedHue < 120) {
                        red = 255 - (adjustedHue * 255 / 120);
                        green = adjustedHue * 255 / 120;
                        blue = 0;
                    }
                    else if (adjustedHue < 240) {
                        red = 0;
                        green = 255 - ((adjustedHue - 120) * 255 / 120);
                        blue = (adjustedHue - 120) * 255 / 120;
                    }
                    else {
                        red = (adjustedHue - 240) * 255 / 120;
                        green = 0;
                        blue = 255 - ((adjustedHue - 240) * 255 / 120);
                    }
                    ioWars.stargate.rgb.setLEDColor(i, red, green, blue);
                }
                ioWars.stargate.rgb.updateLEDs();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                hue = (hue + 10) % 360;
                break;
            }
        }

        // Turn off LEDs after sound stops
        for (int i = 0; i < 12; i++) {
            ioWars.stargate.rgb.setLEDColor(i, 0, 0, 0);
        }
        ioWars.stargate.rgb.updateLEDs();
        });

    // Ensure all threads finish execution
    if (rgbThread.joinable()) rgbThread.join();
    if (soundThread.joinable()) soundThread.join();

    for (int i = 0; i < 3; i++) {
            ioWars.stargate.enable_backlight(false);
            for (int i = 0; i < 12; i++) {
                ioWars.stargate.rgb.setLEDColor(i, 0, 0, 0);
            }
            ioWars.stargate.rgb.updateLEDs();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            ioWars.stargate.enable_backlight(true);
            for (int i = 0; i < 12; i++) {
                ioWars.stargate.rgb.setLEDColor(i, 0, 0, 255);
            }
            ioWars.stargate.rgb.updateLEDs();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    ioWars.stargate.enable_backlight(false);
    for (int i = 0; i < 12; i++) {
        ioWars.stargate.rgb.setLEDColor(i, 0, 0, 0);
    }
    ioWars.stargate.rgb.updateLEDs();

    for (int i = 3; i >= 1; i--) {
        ioWars.stargate.sound.playTone(440*i, 100);
		Sleep(150);
        ioWars.stargate.sound.stop();
    }

    // Turn off Stargate after animation completes
    ioWars.stargate.off();
}


int main() {
    IOWars ioWars;
    system("color 0A");
    //stargateStroboscope(ioWars, 50);
    stargateOpeningAnimation(ioWars);
    system("pause");
    /*
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
    int ignored = _getch();
    ioWars.stargate.ledbar.clear();
    system("pause");
    */

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