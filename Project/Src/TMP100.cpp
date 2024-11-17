// tmp100.cpp -------------------------------------------------------------------------------------
// Implementation file for the TMP100 class.
// ------------------------------------------------------------------------------------------------

#include "tmp100.h"
#include "project_utility.h"

#define TEMPERATURE_REG 0x00
#define CONFIGURATION_REG 0x01

using utility::getI2CReadAddress, utility::getI2CWriteAddress;

// TODO: Remove magic numbers.

// ------------------------------------------------------------------------------------------------
// Public Methods
// ------------------------------------------------------------------------------------------------

TMP100::TMP100(I2C_HandleTypeDef *i2c_handle, uint8_t i2c_address) : i2c_handle(i2c_handle), i2c_address(i2c_address)
{
	HAL_StatusTypeDef status;
	uint8_t config_byte;

	status = this->readConfigurationReg(&config_byte);

	if (status == HAL_OK)
	{
		this->updateResolutionBits(config_byte);
	}
	else
	{
		this->resolution_bits = 0b00;
	}
}

uint8_t TMP100::getResolutionBits()
{
	return this->resolution_bits;
}

HAL_StatusTypeDef TMP100::writeConfigurationReg(uint8_t config_byte)
{
	HAL_StatusTypeDef status;
	uint8_t buffer[2];
	buffer[0] = CONFIGURATION_REG;
	buffer[1] = config_byte;

	status = HAL_I2C_Master_Transmit(
		this->i2c_handle,
		getI2CWriteAddress(this->i2c_address),
		buffer,
		2,
		HAL_MAX_DELAY);

	if (status != HAL_OK)
	{
		return status;
	}

	this->updateResolutionBits(config_byte);

	return HAL_OK;
}

HAL_StatusTypeDef TMP100::triggerOneShotTemperatureConversion()
{
	HAL_StatusTypeDef status;
	uint8_t config_byte;

	status = this->readConfigurationReg(&config_byte);

	if (status != HAL_OK)
	{
		return status;
	}

	if (!(config_byte & 0x01))
	{
		return HAL_ERROR;
	}

	config_byte = config_byte | 0x80;
	status = this->writeConfigurationReg(config_byte);

	if (status != HAL_OK)
	{
		return status;
	}

	HAL_Delay(this->conversion_time_map.at(this->getResolutionBits()));

	return HAL_OK;
}

HAL_StatusTypeDef TMP100::readTemperatureReg(uint16_t *temperature)
{
	if (temperature == nullptr)
	{
		return HAL_ERROR;
	}

	HAL_StatusTypeDef status;
	uint8_t buffer[2] = {0};

	status = this->writePointerReg(TEMPERATURE_REG);

	if (status != HAL_OK)
	{
		return status;
	}

	status = HAL_I2C_Master_Receive(
		this->i2c_handle,
		getI2CReadAddress(this->i2c_address),
		buffer,
		2,
		HAL_MAX_DELAY);

	if (status != HAL_OK)
	{
		return status;
	}

	*temperature = (buffer[0] << 8) | buffer[1];

	return HAL_OK;
}

float TMP100::convertRawTemperatureDataToCelsius(uint16_t raw_temperature_data)
{
	int16_t celsius = raw_temperature_data >> 6;

	if (celsius & 0200)
	{
		celsius = celsius | 0XFC00;
	}

	return celsius * this->resolution_map.at(this->getResolutionBits());
}

// ------------------------------------------------------------------------------------------------
// Private Methods
// ------------------------------------------------------------------------------------------------

void TMP100::updateResolutionBits(uint8_t config_byte)
{
	this->resolution_bits = (config_byte & 0x60) >> 5;
}

HAL_StatusTypeDef TMP100::writePointerReg(uint8_t reg_address)
{
	HAL_StatusTypeDef status;

	status = HAL_I2C_Master_Transmit(
		this->i2c_handle,
		getI2CWriteAddress(this->i2c_address),
		&reg_address,
		1,
		HAL_MAX_DELAY);

	return status;
}

HAL_StatusTypeDef TMP100::readConfigurationReg(uint8_t *config_byte)
{
	if (config_byte == nullptr)
	{
		return HAL_ERROR;
	}

	HAL_StatusTypeDef status;
	uint8_t buffer[1] = {0};

	status = this->writePointerReg(CONFIGURATION_REG);

	if (status != HAL_OK)
	{
		return status;
	}

	status = HAL_I2C_Master_Receive(
		this->i2c_handle,
		getI2CReadAddress(this->i2c_address),
		buffer,
		1,
		HAL_MAX_DELAY);

	if (status != HAL_OK)
	{
		return status;
	}

	*config_byte = buffer[0];

	return HAL_OK;
}

// ------------------------------------------------------------------------------------------------
// Private Static Maps
// ------------------------------------------------------------------------------------------------

const std::unordered_map<uint8_t, float> TMP100::resolution_map = {
	{0b00, 0.5},
	{0b01, 0.25},
	{0b10, 0.125},
	{0b11, 0.0625}};

const std::unordered_map<uint8_t, int> TMP100::conversion_time_map = {
	{0b00, 40},
	{0b01, 80},
	{0b10, 160},
	{0b11, 320}};
