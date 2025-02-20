// I2C.hpp

#ifndef I2C_H
#define I2C_H

// Communication speed declarations (flags)
#define I2C_SPEED_100kbit 0
#define I2C_SPEED_400kbit 1
#define I2C_SPEED_50kbit 2
#define I2C_SPEED_10kbit 3
#define I2C_SPEED_1000kbit 4

// Error code declarations
#define I2C_NO_ERR 0
#define I2C_ERR_WRONG_BYTE_COUNT 1
#define I2C_ERR_NO_START 2
#define I2C_ERR_NACK 3
#define I2C_ERR_BUS 4

class I2C {
public:
    I2C(IOWKIT_HANDLE* deviceHandle, int speed=I2C_SPEED_10kbit);
    ~I2C();
    void write(byte* data);
    void read(byte addr, byte* buffer, int length);

private:
    IOWKIT_HANDLE* DevHandle;
    void enable(bool state, int speed=I2C_SPEED_10kbit);

};

#endif