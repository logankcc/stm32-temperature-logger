// project_main.cpp -------------------------------------------------------------------------------
// Implementation file for the program entry point and core logic.
// ------------------------------------------------------------------------------------------------

#include "project_main.h"
#include "project_utility.h"
#include "tmp100.h"
#include "eeprom.h"

// Program timing
constexpr uint32_t TEN_MINUTE_DELAY_MS = 600000;

using utility::logErrorMessage;

void project_main(I2C_HandleTypeDef *i2c_handle, UART_HandleTypeDef *uart_handle)
{
	HAL_StatusTypeDef status;

	// Initialize the TMP100 temperature sensor assuming ADDO and ADD1 are grounded (binary: 0b01001000)
	uint8_t temperature_sensor_i2c_address = 0x48;
	TMP100 temperature_sensor = TMP100(i2c_handle, temperature_sensor_i2c_address);

	// Configure the TMP100 for Shutdown Mode and a Resolution of 0.25C by setting the SD-bit the and R0-bit HIGH (binary: 0b00100001)
	status = temperature_sensor.writeConfigurationReg(0x21);
	if (status != HAL_OK)
	{
		// Turn off the on-board green LED to indicate configuration failure
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

		const char *message = "Error: Failed to configure TMP100! Terminating program.";
		logErrorMessage(uart_handle, message);
		return;
	}

	// Turn on the on-board green LED to indicate configuration success
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);

	// Initialize the 24FC256 EEPROM assuming ADDO, ADD1, and ADD2 are grounded (binary: 0b01010000)
	// Note: '1010' corresponds to the 4-bit control code
	uint8_t eeprom_i2c_address = 0x50;
	EEPROM eeprom = EEPROM(i2c_handle, eeprom_i2c_address);

	while (1)
	{
		// Trigger a temperature conversion on the TMP100
		status = temperature_sensor.triggerOneShotTemperatureConversion();
		if (status != HAL_OK)
		{
			const char *message = "Error: Failed to trigger One-Shot temperature conversion!";
			logErrorMessage(uart_handle, message);
			continue;
		}

		// Read the raw temperature data from the TMP100
		uint16_t raw_temperature_data;
		status = temperature_sensor.readTemperatureReg(&raw_temperature_data);
		{
			const char *message = "Error: Failed to read temperature data from TMP100!";
			logErrorMessage(uart_handle, message);
			continue;
		}

		 // Write the raw temperature data to the EEPROM
		status = eeprom.writeTwoBytes(raw_temperature_data);
		{
			const char *message = "Error: Failed to write temperature data to EEPROM!";
			logErrorMessage(uart_handle, message);
			continue;
		}

		// Wait for 10 minutes for the next temperature conversion
		HAL_Delay(TEN_MINUTE_DELAY_MS);
	}
}
