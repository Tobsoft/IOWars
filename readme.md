# IOWars Library

## Overview
The IOWars Library is designed to interface with the IOWarrior board, providing functionality to control various components of the Stargate system and other external devices. This library supports the control of LEDs, LCD displays, and additional components using SPI communication and other protocols.

## Features
- **Segment Display (7-segment)**: Control the 7-segment display óf the Stargate via the IOWarrior.
- **RGB LED Control**: Control WS2812 RGB LEDs with precise color management.
- **SPI Communication**: Interface with external components such as the STP16CP05 LED driver.
- **Onboard and External LEDs**: Manage the Stargate's onboard LED bar.
- **LCD Control**: Interface with LCDs using a HD44780 driver chip
- **Stargate integration**: Control most of the Stargates components using the IOWarrior

## Getting Started

### Requirements
- IOWarrior board (e.g., IOWarrior100)
- Supported components: WS2812 RGB LEDs, LCD displays, Stargate components, IOWars Modules
- C++ compiler with support for the IOWarrior API (e.g. Visual Studio for C++)

### Installation
- Clone the repository and add the source files to your project:
  - Use Visual Studios git integration to clone the repository OR

  - ```git clone https://github.com/Tobsoft/IOWars```



- Setup
  - Open the solution (sln) with Visual Studio and start building from there OR
  - Compile the code with all necessary files and link the IOWarrior library

- iowkit installation
  - copy the iowkit.dll from IOWars/lib to C:/Windows/System32/ (Requires Admin rights)

### Usage

#### Initialization
Include the relevant headers in your project:

```cpp
#include "IOWarsWeiT.hpp"
```

Create and initialize components:

```cpp
int main() {
    IOWars ioWars;
}
```

#### Control Examples

**Enable Backlight:**
```cpp
ioWars.stargate.enable_backlight(true);
```

**Set RGB LED Colors:**
```cpp
ioWars.stargate.rgb.setLEDColor(0, 255, 0, 0);  // Set first LED to red
ioWars.stargate.rgb.updateLEDs();
```

**Control Segment Display:**
```cpp
ioWars.stargate.seg7.writeInt(5); // Writes 5 to all displays
ioWars.stargate.seg7.count(); // Counts from 0 to 9 until user input
```

**Control the LED Bar:**
```cpp
ioWars.stargate.ledbar.setSide(true, 0xFF);  // Turn on all left-side LEDs
```

### Notes
- Ensure that the Stargate components are properly connected.
- Handle exceptions during communication with IOWarrior carefully.
- SPI timing and signaling may need fine-tuning for specific components.
- WS2812 LEDs might have different color strenghts, e.g. white might not be (255, 255, 255), but rather (40, 255, 255)

### Contributing
Contributions are welcome! Please fork the repository and submit a pull request with your improvements.

### License
This project is licensed under the Apache 2.0 License. See [LICENSE](./LICENSE.txt) for more details.

### Author

Tobias Weich, 2025

**See also: [HTL_onboard library](https://github.com/Tobsoft/HTL_onboard) for HTL Uno**