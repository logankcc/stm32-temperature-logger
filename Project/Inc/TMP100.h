/**
 * ------------------------------------------------------------------------------------------------
 * @file tmp100.h
 * @brief Header file for the TMP100 class.
 * ------------------------------------------------------------------------------------------------
 */

#pragma once

#include <unordered_map>

#include "stm32f4xx_hal.h"

class TMP100
{
public:
	// Constructor
	TMP100(I2C_HandleTypeDef *i2c_handle, uint8_t i2c_address);

	// Public methods
	HAL_StatusTypeDef writeConfigurationReg(uint8_t config_byte);
	HAL_StatusTypeDef triggerOneShotTemperatureConversion();
	HAL_StatusTypeDef readTemperatureReg(uint16_t *temperature);
	float convertRawTemperatureDataToCelsius(uint16_t raw_temperature_data);

private:
	// Private helper methods
	void updateResolutionBits(uint8_t config_byte);
	HAL_StatusTypeDef writePointerReg(uint8_t reg_address);
	HAL_StatusTypeDef readConfigurationReg(uint8_t *config_byte);

	// Data members
	I2C_HandleTypeDef *i2c_handle;
	uint8_t i2c_address;
	uint8_t resolution_bits;

	// Static constant members
	static const float resolution[4];
	static const int resolution_conversion_time[4];
	static const int resolution_bit_shift[4];
	static const uint16_t resolution_signed_bit[4];
	static const uint16_t resolution_sign_extension[4];
};
