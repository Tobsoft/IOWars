// main.cpp

#include "IOWarsWeiT.hpp"

int main() {
    IOWars ioWars;
    ioWars.stargate.ledbar.setAll(0xFFFF);
    ioWars.stargate.enable_backlight(true);

    printf("To test I2C communication, ");
    system("pause");
    // TODO: I2C Test

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
    ioWars.stargate.sound.playMidi("midi/LaNoyeeEnd.mid");
    printf("Midi played!\n");

    ioWars.lcd.write("Hello, World!");
    return 0;
}
