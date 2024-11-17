#include "utility.h"

namespace utility
{
    uint8_t getI2CReadAddress(uint8_t i2c_address)
    {
        return (i2c_address << 1) | 0x01;
    }

    uint8_t getI2CWriteAddress(uint8_t i2c_address)
    {
        return (i2c_address << 1);
    }
}
