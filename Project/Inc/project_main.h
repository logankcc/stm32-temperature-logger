#pragma once

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

void project_main(I2C_HandleTypeDef *i2c_handle, UART_HandleTypeDef *uart_handle);

#ifdef __cplusplus
}
#endif
