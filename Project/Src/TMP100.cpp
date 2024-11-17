// tmp100.cpp -------------------------------------------------------------------------------------
// Implementation file for the TMP100 class.
// ------------------------------------------------------------------------------------------------

#include "tmp100.h"
#include "project_utility.h"

// Masks for TMP100 configuration bits
constexpr uint8_t SD_BIT_MASK = 0x01;
constexpr uint8_t OS_BIT_MASK = 0x80;
constexpr uint8_t R1R0_BIT_MASK = 0x60;

// TMP100 register addresses
constexpr uint8_t TEMPERATURE_REG = 0x00;
constexpr uint8_t CONFIGURATION_REG = 0x01;


// Bit shift for extracting resolution from the configuration byte
constexpr int RESOLUTION_BIT_SHIFT = 5;

using utility::getI2CReadAddress, utility::getI2CWriteAddress;

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
		sizeof(buffer),
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

	if (!(config_byte & SD_BIT_MASK))
	{
		return HAL_ERROR;
	}

	config_byte = config_byte | OS_BIT_MASK;
	status = this->writeConfigurationReg(config_byte);

	if (status != HAL_OK)
	{
		return status;
	}

	int conversion_time = this->resolution_conversion_time[this->resolution_bits];
	HAL_Delay(conversion_time);

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
		sizeof(buffer),
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
	int shift_amount = this->resolution_bit_shift[this->resolution_bits];
	int16_t celsius = raw_temperature_data >> shift_amount;

	uint16_t signed_bit = this->resolution_signed_bit[this->resolution_bits];
	if (celsius & signed_bit)
	{
		uint16_t sign_extension = this->resolution_sign_extension[this->resolution_bits];
		celsius = celsius | sign_extension;
	}

	return celsius * this->resolution[this->resolution_bits];
}

// ------------------------------------------------------------------------------------------------
// Private Methods
// ------------------------------------------------------------------------------------------------

void TMP100::updateResolutionBits(uint8_t config_byte)
{
	this->resolution_bits = (config_byte & R1R0_BIT_MASK) >> RESOLUTION_BIT_SHIFT;
}

HAL_StatusTypeDef TMP100::writePointerReg(uint8_t reg_address)
{
	HAL_StatusTypeDef status;

	status = HAL_I2C_Master_Transmit(
		this->i2c_handle,
		getI2CWriteAddress(this->i2c_address),
		&reg_address,
		sizeof(reg_address),
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
		sizeof(buffer),
		HAL_MAX_DELAY);

	if (status != HAL_OK)
	{
		return status;
	}

	*config_byte = buffer[0];

	return HAL_OK;
}

// ------------------------------------------------------------------------------------------------
// Static Constants
// ------------------------------------------------------------------------------------------------

const float TMP100::resolution[4] = {0.5, 0.25, 0.125, 0.0625};
const int TMP100::resolution_conversion_time[4] = {40, 80, 160, 320};
const int TMP100::resolution_bit_shift[4] = {7, 6, 5, 4};
const uint16_t TMP100::resolution_signed_bit[4] = {0x0200, 0x0400, 0x0800, 0x1000};
const uint16_t TMP100::resolution_sign_extension[4] = {0xFE00, 0xFC00, 0xF800, 0xF000};
