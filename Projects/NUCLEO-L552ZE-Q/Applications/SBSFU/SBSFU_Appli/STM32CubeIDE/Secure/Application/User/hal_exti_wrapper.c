/*
 * hal_exti_wrapper.c
 *
 *  Created on: May 18, 2021
 *      Author: paul
 */
#include "main.h"



typedef void CMSE_NS_CALL (*hal_gpio_exti_callback_t)(uint16_t);

// can't link against NS, so NS wrapper sets this up at startup
hal_gpio_exti_callback_t rising_c, falling_c;
void CMSE_NS_ENTRY NS_HAL_GPIO_EXTI_setup(void (*r)(uint16_t), void (*f)(uint16_t)) {
  rising_c = (hal_gpio_exti_callback_t) r;
  falling_c = (hal_gpio_exti_callback_t) f;
}

void __attribute__((weak)) S_HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {

}

void __attribute__((weak)) S_HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {

}

// setup to get hal working...
void EXTI13_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

uint32_t down_time;

// hook the Wrapper into HAL
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
  S_HAL_GPIO_EXTI_Rising_Callback(GPIO_Pin);
  if (GPIO_Pin == 13) {
    down_time = HAL_GetTick();
  }
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
  S_HAL_GPIO_EXTI_Falling_Callback(GPIO_Pin);
  if (GPIO_Pin == 13) {
    if (HAL_GetTick() - down_time < 1000) {
      rising_c(13);
      falling_c(13);
    }
  }
}

void HAL_EXTI_WRAPPER_timer_callback() {

}

