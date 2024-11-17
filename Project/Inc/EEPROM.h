#pragma once

#include <cstdint>

#include "stm32f4xx_hal.h"

class EEPROM
{
private:
    I2C_HandleTypeDef *i2c_handle;
    uint8_t i2c_address;
    uint16_t current_write_address = 0x0000;

public:
    EEPROM(I2C_HandleTypeDef *i2c_handle, uint8_t i2c_address) : i2c_handle(i2c_handle), i2c_address(i2c_address) {}

    void buildWriteBuffer(uint8_t *buffer, uint16_t data);

    void buildAddressBuffer(uint8_t *buffer, uint16_t memory_address);

    void writeTwoBytes(uint16_t data);

    uint16_t readTwoBytes(uint16_t memory_address);
};
