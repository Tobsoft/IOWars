// main.cpp

#include "IOWarsWeiT.hpp"

void playMelody(IOWars* ioWars) {
    // notes in the melody:
    int melody[] = {
      NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
    };

    // note durations: 4 = quarter note, 8 = eighth note, etc.:
    int noteDurations[] = {
      4, 8, 8, 4, 4, 4, 4, 4
    };

    // iterate over the notes of the melody:

    for (int thisNote = 0; thisNote < 8; thisNote++) {

        // to calculate the note duration, take one second divided by the note type.

        //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.

        int noteDuration = 500 / noteDurations[thisNote];

        ioWars->stargate.sound.playTone(melody[thisNote], noteDuration);

        // to distinguish the notes, set a minimum time between them.

        // the note's duration + 30% seems to work well:

        int pauseBetweenNotes = noteDuration * 1.30;

        Sleep(pauseBetweenNotes);

        // stop the tone playing:

        ioWars->stargate.sound.stop();
    }
}

int main() {
    IOWars ioWars;
    ioWars.stargate.ledbar.setAll(0xFFFF);
    ioWars.stargate.enable_backlight(true);

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
    playMelody(&ioWars);

    ioWars.lcd.write("Hello, World!");
    return 0;
}
