/**
 * ------------------------------------------------------------------------------------------------
 * @file eeprom.cpp
 * @brief Implementation file for the EEPROM class.
 * ------------------------------------------------------------------------------------------------
 */

#include "eeprom.h"
#include "project_utility.h"

// EEPROM address range
constexpr uint16_t EEPROM_MIN_ADDRESS = 0x0000;
constexpr uint16_t EEPROM_MAX_ADDRESS = 0x7FFF;

// EEPROM timing
constexpr uint32_t EEPROM_WRITE_CYCLE_DELAY_MS = 5;

using utility::getI2CReadAddress, utility::getI2CWriteAddress;

/**
 * ------------------------------------------------------------------------------------------------
 * @section Public_Methods Public Methods
 * ------------------------------------------------------------------------------------------------
 */

/**
 * @brief Constructs an EEPROM object with the specified I2C handle and address.
 * @param i2c_handle Pointer to the I2C handle used for communication.
 * @param i2c_address The 7-bit I2C address of the EEPROM device.
 */
EEPROM::EEPROM(I2C_HandleTypeDef *i2c_handle, uint8_t i2c_address) : i2c_handle(i2c_handle), i2c_address(i2c_address)
{
    this->current_write_address = EEPROM_MIN_ADDRESS;
}

/**
 * @brief Builds a four-byte write buffer containing the current EEPROM write address and 16-bit data value.
 * @param buffer Pointer to a buffer where the address and data will be stored.
 * @param data The 16-bit data to be written to the EEPROM.
 */
void EEPROM::buildWriteBuffer(uint8_t *buffer, uint16_t data)
{
    buffer[0] = this->current_write_address >> 8;
    buffer[1] = this->current_write_address;
    buffer[2] = data >> 8;
    buffer[3] = data;
}

/**
 * @brief Builds a two-byte address buffer containing the EEPROM memory address.
 * @param address_buffer Pointer to a buffer where the address will be stored.
 * @param memory_address The 16-bit memory address to read from.
 */

void EEPROM::buildAddressBuffer(uint8_t *address_buffer, uint16_t memory_address)
{
    address_buffer[0] = memory_address >> 8;
    address_buffer[1] = memory_address;
}

/**
 * @brief Writes two bytes of data to the EEPROM at the current write address.
 * @param data The 16-bit data value to be written to the EEPROM.
 * @return The HAL status of the I2C transmission.
 */
HAL_StatusTypeDef EEPROM::writeTwoBytes(uint16_t data)
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
        return status;
    }

    this->current_write_address++;
    if (this->current_write_address > EEPROM_MAX_ADDRESS)
    {
        this->current_write_address = EEPROM_MIN_ADDRESS;
    }

    HAL_Delay(EEPROM_WRITE_CYCLE_DELAY_MS);

    return HAL_OK;
}

/**
 * @brief Reads two bytes of data from the specified EEPROM memory address.
 * @param memory_address The 16-bit valid memory address (0x0000 to 0x7FFF) to read from.
 * @param data Pointer to a 16-bit variable where the read data will be stored.
 * @return The HAL status of the I2C transmission.
 */
HAL_StatusTypeDef EEPROM::readTwoBytes(uint16_t memory_address, uint16_t *data)
{
    if (memory_address < EEPROM_MIN_ADDRESS || memory_address > EEPROM_MAX_ADDRESS)
    {
        return HAL_ERROR;
    }

    if (data == nullptr)
    {
        return HAL_ERROR;
    }

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
        return status;
    }

    status = HAL_I2C_Master_Receive(
        this->i2c_handle,
        getI2CReadAddress(this->i2c_address),
        buffer,
        sizeof(buffer),
        HAL_MAX_DELAY);

    if (status != HAL_OK)
    {
        return status;
    }

    *data = (buffer[0] << 8) | buffer[1];

    return HAL_OK;
}
