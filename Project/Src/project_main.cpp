#include "project_main.h"
#include "TMP100.h"
#include "EEPROM.h"

constexpr uint32_t DELAY = 600000;

void project_main(I2C_HandleTypeDef *i2c_handle, UART_HandleTypeDef *uart_handle)
{
	HAL_StatusTypeDef status;

	uint8_t temperature_sensor_i2c_address = 0x48;
	TMP100 temperature_sensor = TMP100(i2c_handle, temperature_sensor_i2c_address);

	status = temperature_sensor.writeConfigurationReg(0x21);

	if (status != HAL_OK)
	{
		// TODO: Log error.
		return; // Abort if configuration fails
	}

	uint8_t eeprom_i2c_address = 0x50;
	EEPROM eeprom = EEPROM(i2c_handle, eeprom_i2c_address);

	while (1)
	{
		// Trigger a conversion
		status = temperature_sensor.triggerOneShotTemperatureConversion();
		if (status != HAL_OK)
		{
			// TODO: Log error.
			continue; // Skip this loop iteration
		}

		// Read the temp data
		uint16_t raw_temperature_data;
		status = temperature_sensor.readTemperatureReg(&raw_temperature_data);
		{
			// TODO: Log error.
			continue; // Skip this loop iteration
		}

		// Store the temp data
		status = eeprom.writeTwoBytes(raw_temperature_data);
		{
			// TODO: Log error.
			continue; // Skip this loop iteration
		}

		// Wait 10 minutes (i.e. 600000 ms);
		HAL_Delay(DELAY);
	}
}
