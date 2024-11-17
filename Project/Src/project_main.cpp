#include "project_main.h"
#include "TMP100.h"
#include "EEPROM.h"

constexpr uint32_t DELAY = 600000;

void project_main(I2C_HandleTypeDef *i2c_handle)
{

	uint8_t temperature_sensor_i2c_address = 0x48;
	TMP100 temperature_sensor = TMP100(i2c_handle, temperature_sensor_i2c_address);

	temperature_sensor.writeConfigurationReg(0x21);

	uint8_t eeprom_i2c_address = 0x50;
	EEPROM eeprom = EEPROM(i2c_handle, eeprom_i2c_address);

	while (1)
	{
		// Trigger a conversion
		temperature_sensor.triggerOneShotTemperatureConversion();

		// Read the temp data
		uint16_t raw_temperature_data = temperature_sensor.readTemperatureReg();

		// Store the temp data
		eeprom.writeTwoBytes(raw_temperature_data);

		// Wait 10 minutes (i.e. 600000 ms);
		HAL_Delay(DELAY);
	}
}
