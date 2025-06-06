// Sound.hpp

#ifndef SOUND_H
#define SOUND_H

#include "pitches.h"
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>

class Sound {
public:
    Sound(IOWKIT_HANDLE* deviceHandle);
    ~Sound();

    // Play a tone with specific frequency and duration (non-blocking)
    void playTone(float frequency, float durationMs);

    // Play a midi file
    void playMidi(const char* midiFile);

    // Stop playback immediately
    void stop();

private:
    IOWKIT_HANDLE* DevHandle;
    std::atomic<bool> playing;  // Tracks if playback is ongoing
    std::thread toneThread; // Thread for non-blocking playback

    // Helper for enabling or disabling the sound
    void enable(bool state);

    // Set PWM frequency for playback
    void setPWMFrequency(float frequency);
    void configurePWM(uint16_t prescaler, uint16_t cycle, uint16_t pulseWidth);
};

#endif
