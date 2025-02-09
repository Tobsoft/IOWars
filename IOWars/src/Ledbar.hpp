// Ledbar.hpp

#ifndef LEDBAR_H
#define LEDBAR_H

#include "SPI.hpp"
#include <cstdint>

class Ledbar {
public:
  Ledbar(SPI* spi);
  ~Ledbar();

  // Set the entire 16-bit LED bar pattern
  void setAll(uint16_t pattern);

  // Set either the left (high byte) or right (low byte) side based on the `left` flag
  void setSide(bool left, uint8_t pattern);

  // Turn all LEDs off
  void clear();

private:
  SPI* spi;  // Pointer to SPI object for communication
};

#endif
