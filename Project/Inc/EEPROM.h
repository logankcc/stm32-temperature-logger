// eeprom.h ---------------------------------------------------------------------------------------
// Header file for the EEPROM class.
// ------------------------------------------------------------------------------------------------

#pragma once

#include "stm32f4xx_hal.h"

class EEPROM
{
public:
    // Constructor
    EEPROM(I2C_HandleTypeDef *i2c_handle, uint8_t i2c_address);

    // Public methods
    void buildWriteBuffer(uint8_t *buffer, uint16_t data);
    void buildAddressBuffer(uint8_t *buffer, uint16_t memory_address);
    HAL_StatusTypeDef writeTwoBytes(uint16_t data);
    HAL_StatusTypeDef readTwoBytes(uint16_t memory_address, uint16_t *data);

private:
    // Data members
    I2C_HandleTypeDef *i2c_handle;
    uint8_t i2c_address;
    uint16_t current_write_address;
};
