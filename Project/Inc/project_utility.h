/**
 * ------------------------------------------------------------------------------------------------
 * @file project_utility.h
 * @brief Header file to define common utility functions.
 * ------------------------------------------------------------------------------------------------
 */

#pragma once

#include <cstdint>

namespace utility
{
    uint8_t getI2CReadAddress(uint8_t i2c_address);

    uint8_t getI2CWriteAddress(uint8_t i2c_address);

    void logMessage(UART_HandleTypeDef *uart_handle, char *message);

    void logStatusMessage(UART_HandleTypeDef *uart_handle, char *status_message);

    void scanI2CAddresses(I2C_HandleTypeDef *i2c_handle, UART_HandleTypeDef *uart_handle);
}
