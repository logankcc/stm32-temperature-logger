// project_utility.h ------------------------------------------------------------------------------
// Header file to define common utility functions.
// ------------------------------------------------------------------------------------------------

#pragma once

#include <cstdint>

namespace utility
{
    uint8_t getI2CReadAddress(uint8_t i2c_address);

    uint8_t getI2CWriteAddress(uint8_t i2c_address);

    void logErrorMessage(UART_HandleTypeDef *uart_handle, const char *error_message);
}
