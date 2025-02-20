// DS1307.cpp

#include "IOWarsWeiT.hpp"
#include "I2C.hpp"
#include "DS1307.hpp"

void DS1307::setTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    uint8_t data[3] = { decToBcd(seconds), decToBcd(minutes), decToBcd(hours) };
    i2c.write(data);
}

void DS1307::getTime(uint8_t& hours, uint8_t& minutes, uint8_t& seconds) {
    // Request time


    // Read time
    uint8_t data[3];
    i2c.read(DS1307_ADDRESS, data, 3);
    seconds = bcdToDec(data[0]);
    minutes = bcdToDec(data[1]);
    hours = bcdToDec(data[2]);
}

void DS1307::setDate(uint8_t day, uint8_t month, uint8_t year) {
    uint8_t data[3] = { decToBcd(day), decToBcd(month), decToBcd(year) };
    i2c.write(data);
}

void DS1307::getDate(uint8_t& day, uint8_t& month, uint8_t& year) {
    uint8_t data[3];
    i2c.read(DS1307_ADDRESS, data, 3);
    day = bcdToDec(data[0]);
    month = bcdToDec(data[1]);
    year = bcdToDec(data[2]);
}