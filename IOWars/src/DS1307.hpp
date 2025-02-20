// DS1307.hpp

#ifndef DS1307_H
#define DS1307_H

#include <cstdint>

#define DS1307_ADDRESS 0x68  // I2C address of DS1307 RTC

class DS1307 {
public:
  DS1307(I2C& i2cInstance) : i2c(i2cInstance) {}

  // Set time (24-hour format)
  void setTime(uint8_t hours, uint8_t minutes, uint8_t seconds);

  // Read time
  void getTime(uint8_t& hours, uint8_t& minutes, uint8_t& seconds);

  // Set date
  void setDate(uint8_t day, uint8_t month, uint8_t year);

  // Read date
  void getDate(uint8_t& day, uint8_t& month, uint8_t& year);

private:
  I2C& i2c;

  // Convert Binary-Coded Decimal (BCD) to decimal
  uint8_t bcdToDec(uint8_t bcd) { return (bcd >> 4) * 10 + (bcd & 0x0F); }

  // Convert decimal to Binary-Coded Decimal (BCD)
  uint8_t decToBcd(uint8_t dec) { return ((dec / 10) << 4) | (dec % 10); }
};

#endif
