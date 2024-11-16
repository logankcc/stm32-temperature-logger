#include "project_main.h"

void project_main() {

	while (1) {
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		HAL_Delay(1000);
	}
}
