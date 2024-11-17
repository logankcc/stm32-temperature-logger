#include "EEPROM.h"
#include "utility.h"

#define EEPROM_MAX_ADDRESS 0x7FFF
#define EEPROM_WRITE_CYCLE_DELAY 5

using utility::getI2CReadAddress, utility::getI2CWriteAddress;

// TODO: Remove magic numbers.

void EEPROM::buildWriteBuffer(uint8_t *buffer, uint16_t data)
{
    buffer[0] = this->current_write_address >> 8;
    buffer[1] = this->current_write_address;
    buffer[2] = data >> 8;
    buffer[3] = data;
}

void EEPROM::buildAddressBuffer(uint8_t *address_buffer, uint16_t memory_address)
{
    address_buffer[0] = memory_address >> 8;
    address_buffer[1] = memory_address;
}

void EEPROM::writeTwoBytes(uint16_t data)
{
    HAL_StatusTypeDef status;
    uint8_t buffer[4];

    buildWriteBuffer(buffer, data);

    status = HAL_I2C_Master_Transmit(
        this->i2c_handle,
        getI2CWriteAddress(this->i2c_address),
        buffer,
        sizeof(buffer),
        HAL_MAX_DELAY);

    if (status != HAL_OK)
    {
        // TODO: Handle error.
    }

    this->current_write_address++;
    if (this->current_write_address > EEPROM_MAX_ADDRESS)
    {
        this->current_write_address = 0x0000;
    }

    HAL_Delay(EEPROM_WRITE_CYCLE_DELAY);
}

uint16_t EEPROM::readTwoBytes(uint16_t memory_address)
{
    HAL_StatusTypeDef status;
    uint8_t address_buffer[2];
    uint8_t buffer[2] = {0};

    buildAddressBuffer(address_buffer, memory_address);

    status = HAL_I2C_Master_Transmit(
        this->i2c_handle,
        getI2CWriteAddress(this->i2c_address),
        address_buffer,
        sizeof(address_buffer),
        HAL_MAX_DELAY);

    if (status != HAL_OK)
    {
        // TODO: Handle error.
    }

    status = HAL_I2C_Master_Receive(
        this->i2c_handle,
        getI2CReadAddress(this->i2c_address),
        buffer,
        sizeof(buffer),
        HAL_MAX_DELAY);

    if (status != HAL_OK)
    {
        // TODO: Handle error.
    }

    return (buffer[0] << 8) | buffer[1];
}