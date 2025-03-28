// DS1307.cpp

#include "IOWarsWeiT.hpp"
#include "I2C.hpp"
#include "DS1307.hpp"

void DS1307::setTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    uint8_t data[3] = { decToBcd(seconds), decToBcd(minutes), decToBcd(hours) };
    i2c.write(data, sizeof(data));
}

void DS1307::getDateTime(dateTime& time) {
    // Set register Pointer
    byte data[2] = { (DS1307_ADDRESS << 1), 0x00 };
    i2c.write(data, sizeof(data));

    // Set RTC to Read mode
    byte readMode[1] = { (DS1307_ADDRESS << 1) | 0x01 };
    i2c.write(readMode, sizeof(readMode));

    // Read dateTime
    i2c.read(DS1307_ADDRESS, &time.seconds, 1);
    time.seconds = bcdToDec(time.seconds & 0x7F);

    i2c.read(DS1307_ADDRESS, &time.minutes, 1);
    time.minutes = bcdToDec(time.minutes);

    i2c.read(DS1307_ADDRESS, &time.hours, 1);
    time.hours = bcdToDec(time.hours);

    i2c.read(DS1307_ADDRESS, &time.Wday, 1);
    time.Wday = bcdToDec(time.Wday);

    i2c.read(DS1307_ADDRESS, &time.day, 1);
    time.day = bcdToDec(time.day);

    i2c.read(DS1307_ADDRESS, &time.month, 1);
    time.month = bcdToDec(time.month);

    i2c.read(DS1307_ADDRESS, &time.year, 1);
    time.year = bcdToDec(time.year);
}


void DS1307::setDate(uint8_t day, uint8_t month, uint8_t year) {
    uint8_t data[3] = { decToBcd(day), decToBcd(month), decToBcd(year) };
    i2c.write(data, sizeof(data));
}

void DS1307::getDate(uint8_t& day, uint8_t& month, uint8_t& year) {
    uint8_t data[3];
    i2c.read(DS1307_ADDRESS, data, 3);
    day = bcdToDec(data[0]);
    month = bcdToDec(data[1]);
    year = bcdToDec(data[2]);
}