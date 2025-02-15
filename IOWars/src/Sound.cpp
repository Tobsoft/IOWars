#include "IOWarsWeiT.hpp"
#include "Stargate.hpp"
#include "helper.hpp"

#include "Sound.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <cmath>

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
    if (frequency == 0.0f) {
        stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(durationMs)));
        return;
    }

    setPWMFrequency(frequency);
    playing = true;

    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(durationMs)));

    stop();
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

    // Calculate prescaler and cycle length using floating point precision
    float prescaler = baseClock / (frequency * 65536.0f);
    uint16_t cycleLength = static_cast<uint16_t>(baseClock / ((prescaler + 1.0f) * frequency));
    if (cycleLength > 65535) cycleLength = 65535;
    uint16_t pulseWidth = static_cast<uint16_t>(cycleLength / 2.0f);

    configurePWM(static_cast<uint16_t>(prescaler), cycleLength, pulseWidth);
}
