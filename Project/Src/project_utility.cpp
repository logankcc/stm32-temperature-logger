/**
 * @file project_utility.cpp
 * @brief Implementation file for common utility functions.
 */

#include <stdio.h>
#include <string.h>

#include "stm32f4xx_hal.h"

#include "project_utility.h"

constexpr size_t UART_BUFFER_SIZE = 64;

namespace utility
{
    /**
     * @brief Calculates the 8-bit I2C read address from a 7-bit I2C address.
     * @param i2c_address specifies the 7-bit I2C address of the device.
     * @return The 8-bit I2C read address.
     */
    uint8_t getI2CReadAddress(uint8_t i2c_address)
    {
        return (i2c_address << 1) | 0x01;
    }

    /**
     * @brief Calculates the 8-bit I2C write address from a 7-bit I2C address.
     * @param i2c_address specifies the 7-bit I2C address of the device.
     * @return The 8-bit I2C write address.
     */
    uint8_t getI2CWriteAddress(uint8_t i2c_address)
    {
        return (i2c_address << 1);
    }

    /**
     * @brief Transmits a message via UART.
     * @param uart_handle points to the UART handle used for transmission.
     * @param message specifies the null-terminated error message to transmit.
     */
    void logMessage(UART_HandleTypeDef *uart_handle, const char *message)
    {
        if (uart_handle && message)
        {
            HAL_UART_Transmit(uart_handle, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
        }
    }

    /**
     * @brief Logs an error message via UART.
     * @param uart_handle points to the UART handle used for transmission.
     * @param error_message specifies the null-terminated error message to transmit.
     */
    void logErrorMessage(UART_HandleTypeDef *uart_handle, const char *error_message)
    {
        char uart_buffer[UART_BUFFER_SIZE];
        size_t message_length = strlen(error_message);

        if (message_length >= sizeof(uart_buffer))
        {
            strncpy(uart_buffer, error_message, sizeof(uart_buffer) - 1);
            uart_buffer[sizeof(uart_buffer) - 1] = '\0';
        }
        else
        {
            strcpy(uart_buffer, error_message);
        }

        logMessage(uart_handle, uart_buffer);
    }

    /**
     * @brief Scans the I2C bus (0x08 to 0x77) for connected devices and logs their addresses via UART.
     * @param i2c_handle points to the I2C handle used for communication.
     * @param uart_handle points to the UART handle used for transmission.
     */
    void scanI2CAddresses(I2C_HandleTypeDef *i2c_handle, UART_HandleTypeDef *uart_handle)
    {
        char uart_buffer[UART_BUFFER_SIZE];
        int uart_buffer_length;

        logMessage(uart_handle, "Scanning I2C bus...\r\n");

        for (uint8_t i2c_address = 0x08; i2c_address <= 0x77; i2c_address++)
        {
            HAL_StatusTypeDef result = HAL_I2C_IsDeviceReady(i2c_handle, getI2CReadAddress(i2c_address), 1, 10);

            if (result == HAL_OK)
            {
                uart_buffer_length = snprintf(uart_buffer, sizeof(uart_buffer), "I2C device found at address 0x%02X\r\n", i2c_address);
                logMessage(uart_handle, uart_buffer);
            }
        }

        logMessage(uart_handle, "Scan complete.\r\n");
    }
}
