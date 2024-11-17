// porject_main.h ---------------------------------------------------------------------------------
// Header file to define the program entry point.
// ------------------------------------------------------------------------------------------------

#pragma once

#include "stm32f4xx_hal.h"

// Ensure C++ compilers treat the following declaration as C code to prevent name mangling
#ifdef __cplusplus
extern "C" {
#endif

void project_main(I2C_HandleTypeDef *i2c_handle, UART_HandleTypeDef *uart_handle);

#ifdef __cplusplus
}
#endif
