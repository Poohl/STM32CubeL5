
#include "hal_systick_wrapper.h"

#include "stm32l5xx_hal.h"
#include "hal_gpio_wrapper_s.h"
#include "hal_exti_wrapper_s.h"

__attribute__((weak)) void HAL_SYSTICK_Callback_A(void) {
}

void HAL_SYSTICK_Callback(void) {
  HAL_GPIO_WRAPPER_timer_callback();
  HAL_EXTI_WRAPPER_timer_callback
  HAL_SYSTICK_Callback_A();
}
