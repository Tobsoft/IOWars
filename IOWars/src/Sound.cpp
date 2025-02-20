// Sound.cpp

#include "IOWarsWeiT.hpp"
#include "Stargate.hpp"

#include "Sound.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <cmath>

struct Note {
    float frequency;
    float durationMs;
};

Sound::Sound(IOWKIT_HANDLE* deviceHandle) : DevHandle(deviceHandle), playing(false) {

}

Sound::~Sound() {

}

void Sound::enable(bool state) {
    IOWKIT100_SPECIAL_REPORT enableConfig;
    memset(&enableConfig, 0x00, IOWKIT100_SPECIAL_REPORT_SIZE);
    enableConfig.ReportID = 0x20;
    enableConfig.Bytes[0] = state ? 2 : 0;

    IowKitWrite(*DevHandle, IOW_PIPE_SPECIAL_MODE, (PCHAR)&enableConfig, IOWKIT100_SPECIAL_REPORT_SIZE);
}

void Sound::playTone(float frequency, float durationMs) {
    playing = true;
    toneThread = std::thread([this, frequency, durationMs]() {
        if (frequency == 0.0f) {
            stop();
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(durationMs)));
            return;
        }

        setPWMFrequency(frequency);
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(durationMs)));
        });

    toneThread.detach(); // Detach the thread to allow independent execution
}

void Sound::stop() {
    playing = false;
    configurePWM(0, 0, 0);  // Disable PWM output
    enable(false);
}

void Sound::configurePWM(uint16_t prescaler, uint16_t cycle, uint16_t pulseWidth) {
    enable(true);
    IOWKIT100_SPECIAL_REPORT pwmConfig;
    memset(&pwmConfig, 0x00, IOWKIT100_SPECIAL_REPORT_SIZE);
    pwmConfig.ReportID = 0x21;
    pwmConfig.Bytes[0] = 0x00;
    pwmConfig.Bytes[1] = prescaler & 0xFF;
    pwmConfig.Bytes[2] = (prescaler >> 8) & 0xFF;
    pwmConfig.Bytes[3] = cycle & 0xFF;
    pwmConfig.Bytes[4] = (cycle >> 8) & 0xFF;
    // Channel 1 is SND (7/8)
    pwmConfig.Bytes[7] = pulseWidth & 0xFF;
    pwmConfig.Bytes[8] = (pulseWidth >> 8) & 0xFF;

    IowKitWrite(*DevHandle, IOW_PIPE_SPECIAL_MODE, (PCHAR)&pwmConfig, IOWKIT100_SPECIAL_REPORT_SIZE);
}

void Sound::setPWMFrequency(float frequency) {
    if (frequency == 0.0f) {
        stop();
        return;
    }

    const uint32_t baseClock = 48000000; // 48MHz

    // Calculate prescaler and cycle length
    uint16_t prescaler = baseClock / (frequency * 65536.0f);
    uint16_t cycleLength = static_cast<uint16_t>(baseClock / ((prescaler + 1.0f) * frequency));
    if (cycleLength > 65535) cycleLength = 65535;
    uint16_t pulseWidth = static_cast<uint16_t>(cycleLength / 2.0f);

    configurePWM(prescaler, cycleLength, pulseWidth);
}

void Sound::playMidi(const char* midiFile) {
    char command[256];
    const char* pythonScript = "src\\py\\midi_to_freq.py";
    snprintf(command, sizeof(command), "python3 %s \"%s\"", pythonScript, midiFile);
    system(command);

    // Open the converted frequency file
    char freqFile[256];
    std::string midiPath(midiFile);
    size_t dotPos = midiPath.find_last_of('.');
    if (dotPos != std::string::npos) {
        midiPath = midiPath.substr(0, dotPos);  // Remove the .mid extension
    }
    snprintf(freqFile, sizeof(freqFile), "%s.freq", midiPath.c_str());
    FILE* file;
    errno_t err = fopen_s(&file, freqFile, "r");
    if (err != 0 || !file) {
        printf("Failed to open %s\n", freqFile);
        return;
    }

    std::vector<Note> notes;
    float freq, duration;
    while (fscanf_s(file, "%f %f", &freq, &duration) == 2) {
        notes.push_back({ freq, duration });
    }
    fclose(file);

    // Play parsed notes
    for (const auto& note : notes) {
        playTone(note.frequency, note.durationMs);
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(note.durationMs)));
    }
    stop();
}