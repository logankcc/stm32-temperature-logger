#include "TMP100.h"
#include "utility.h"

#define TEMPERATURE_REG 0x00
#define CONFIGURATION_REG 0x01

using utility::getI2CReadAddress, utility::getI2CWriteAddress;

// TODO: Remove magic numbers.

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

void TMP100::updateResolutionBits(uint8_t config_byte)
{
	this->resolution_bits = (config_byte & 0x60) >> 5;
}

void TMP100::writePointerReg(uint8_t reg_address)
{
	HAL_StatusTypeDef status;

	status = HAL_I2C_Master_Transmit(
		this->i2c_handle,
		getI2CWriteAddress(this->i2c_address),
		&reg_address,
		1,
		HAL_MAX_DELAY);

	if (status != HAL_OK)
	{
		// TODO: Handle error.
	}
}

uint8_t TMP100::readConfigurationReg()
{
	HAL_StatusTypeDef status;
	uint8_t buffer[1] = {0};

	this->writePointerReg(CONFIGURATION_REG);

	status = HAL_I2C_Master_Receive(
		this->i2c_handle,
		getI2CReadAddress(this->i2c_address),
		buffer,
		1,
		HAL_MAX_DELAY);

	if (status != HAL_OK)
	{
		// TODO: Handle error.
	}

	return buffer[0];
}

TMP100::TMP100(I2C_HandleTypeDef *i2c_handle, uint8_t i2c_address) : i2c_handle(i2c_handle), i2c_address(i2c_address)
{
	uint8_t config_byte = this->readConfigurationReg();
	this->updateResolutionBits(config_byte);
}

uint8_t TMP100::getResolutionBits()
{
	return this->resolution_bits;
}

void TMP100::writeConfigurationReg(uint8_t config_byte)
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
		// TODO: Handle error.
	}

	this->updateResolutionBits(config_byte);
}

void TMP100::triggerOneShotTemperatureConversion()
{
	uint8_t config_byte = this->readConfigurationReg();

	if (!(config_byte & 0x01))
	{
		// TODO: Handle case where TMP100 is not in shutdown mode
	}

	config_byte = config_byte | 0x80;
	this->writeConfigurationReg(config_byte);

	HAL_Delay(this->conversion_time_map.at(this->getResolutionBits()));
}

uint16_t TMP100::readTemperatureReg()
{
	HAL_StatusTypeDef status;
	uint8_t buffer[2] = {0};

	this->writePointerReg(TEMPERATURE_REG);

	status = HAL_I2C_Master_Receive(
		this->i2c_handle,
		getI2CReadAddress(this->i2c_address),
		buffer,
		2,
		HAL_MAX_DELAY);

	if (status != HAL_OK)
	{
		// TODO: Handle error.
	}

	return (buffer[0] << 8) | buffer[1];
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
