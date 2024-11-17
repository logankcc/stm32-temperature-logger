#pragma once

#include <cstdint>

namespace utility
{
    uint8_t getI2CReadAddress(uint8_t i2c_address);

    uint8_t getI2CWriteAddress(uint8_t i2c_address);
}
