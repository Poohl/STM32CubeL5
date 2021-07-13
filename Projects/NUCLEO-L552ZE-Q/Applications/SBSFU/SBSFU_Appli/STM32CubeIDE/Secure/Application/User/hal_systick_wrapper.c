/*
 * hal_systick_wrapper.c
 *
 *  Created on: Jul 12, 2021
 *      Author: paul
 */

#include "stm32l5xx_hal.h"
#include "main.h"

#define WRAPPER
#include "hal_systick_wrapper.h"
#include "hal_exti_wrapper_s.h"
#include "hal_gpio_wrapper_s.h"

void HAL_SYSTICK_Callback(void) {
  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
  HAL_GPIO_WRAPPER_timer_callback();
  HAL_EXTI_WRAPPER_timer_callback();
  HAL_SYSTICK_Callback_A();
}
