// project_utility.cpp ----------------------------------------------------------------------------
// Implementation file for common utility functions.
// ------------------------------------------------------------------------------------------------

#include <string.h>

#include "stm32f4xx_hal.h"

#include "project_utility.h"

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

    void logErrorMessage(UART_HandleTypeDef *uart_handle, const char *error_message)
    {
        HAL_UART_Transmit(uart_handle, (uint8_t *)error_message, strlen(error_message), HAL_MAX_DELAY);
    }
}
