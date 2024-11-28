/**
 * ------------------------------------------------------------------------------------------------
 * @file project_main.cpp
 * @brief Implementation file for the program entry point and core logic.
 * ------------------------------------------------------------------------------------------------
 */

#include <stdio.h>

#include "project_main.h"
#include "project_utility.h"
#include "tmp100.h"
#include "eeprom.h"

// Program timing
constexpr uint32_t TEN_SECOND_DELAY_MS = 10000;
constexpr uint32_t TEN_MINUTE_DELAY_MS = 600000;

// Buffer size
constexpr size_t UART_BUFFER_SIZE = 64;

using utility::logStatusMessage;

void project_main(I2C_HandleTypeDef *i2c_handle, UART_HandleTypeDef *uart_handle)
{
	HAL_StatusTypeDef status;
	char status_message[64];

	// Initialize the TMP100 temperature sensor assuming ADDO and ADD1 are grounded (binary: 0b01001000)
	uint8_t temperature_sensor_i2c_address = 0x48;
	TMP100 temperature_sensor = TMP100(i2c_handle, temperature_sensor_i2c_address);

	// Configure the TMP100 for Shutdown Mode and a Resolution of 0.25C by setting the SD-bit the and R0-bit HIGH (binary: 0b00100001)
	status = temperature_sensor.writeConfigurationReg(0x21);
	if (status != HAL_OK)
	{
		// Turn off the on-board green LED to indicate configuration failure
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

		snprintf(status_message, sizeof(status_message), "Error: Failed to configure TMP100! Terminating program.\r\n");
		logStatusMessage(uart_handle, status_message);
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
			snprintf(status_message, sizeof(status_message), "Error: Failed to trigger One-Shot temperature conversion!\r\n");
			logStatusMessage(uart_handle, status_message);
			continue;
		}

		// Read the raw temperature data from the TMP100
		uint16_t raw_temperature_data;
		status = temperature_sensor.readTemperatureReg(&raw_temperature_data);
		if (status != HAL_OK)
		{
			snprintf(status_message, sizeof(status_message), "Error: Failed to read temperature data from TMP100!\r\n");
			logStatusMessage(uart_handle, status_message);
			continue;
		}

		// Convert raw temperature data to Celsius and log the result
		float celsius_temperature_data = temperature_sensor.convertRawTemperatureDataToCelsius(raw_temperature_data);
		snprintf(status_message, sizeof(status_message), "Current Temperature: %.2f°C.\r\n", celsius_temperature_data);
		logStatusMessage(uart_handle, status_message);

		// Get the current write address for the EEPROM
		uint16_t current_address = eeprom.getCurrentWriteAddress();

		// Write the raw temperature data to the EEPROM
		status = eeprom.writeTwoBytes(raw_temperature_data);
		if (status != HAL_OK)
		{
			snprintf(status_message, sizeof(status_message), "Error: Failed to write temperature data to EEPROM!\r\n");
			logStatusMessage(uart_handle, status_message);
			continue;
		}

		// Log the memory write result
		snprintf(status_message, sizeof(status_message), "Wrote Raw Temperature Data 0x%04X to EEPROM address 0x%04X.\r\n", raw_temperature_data, current_address);
		logStatusMessage(uart_handle, status_message);

		// Read the raw temperature data from the EEPROM
		status = eeprom.readTwoBytes(current_address, &raw_temperature_data);
		if (status != HAL_OK)
		{
			snprintf(status_message, sizeof(status_message), "Error: Failed to read temperature data from EEPROM!\r\n");
			logStatusMessage(uart_handle, status_message);
			continue;
		}

		// Log the memory read result
		snprintf(status_message, sizeof(status_message), "Read Raw Temperature Data 0x%04X from EEPROM address 0x%04X.\r\n", raw_temperature_data, current_address);
		logStatusMessage(uart_handle, status_message);

		// Wait for 10 seconds for the next temperature conversion
		HAL_Delay(TEN_SECOND_DELAY_MS);
	}
}
