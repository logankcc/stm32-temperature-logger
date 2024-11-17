#pragma once

#include <cstdint>
#include <unordered_map>

#include "stm32f4xx_hal.h"

class TMP100
{
private:
	// Data members
	I2C_HandleTypeDef *i2c_handle;
	uint8_t i2c_address;
	uint8_t resolution_bits;

	// Static maps for resolutions and conversion times
	static const std::unordered_map<uint8_t, float> resolution_map;
	static const std::unordered_map<uint8_t, int> conversion_time_map;

	// Private utility methods
	void updateResolutionBits(uint8_t config_byte);
	void writePointerReg(uint8_t reg_address);
	uint8_t readConfigurationReg();

public:
	// Constructor
	TMP100(I2C_HandleTypeDef *i2c_handle, uint8_t i2c_address);

	// Public methods
	uint8_t getResolutionBits();
	void writeConfigurationReg(uint8_t config_byte);
	void triggerOneShotTemperatureConversion();
	uint16_t readTemperatureReg();
	float convertRawTemperatureDataToCelsius(uint16_t raw_temperature_data);
};
