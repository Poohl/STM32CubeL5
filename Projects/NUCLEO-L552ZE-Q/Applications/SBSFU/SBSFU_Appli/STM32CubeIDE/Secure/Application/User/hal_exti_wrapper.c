/*
 * hal_exti_wrapper.c
 *
 *  Created on: May 18, 2021
 *      Author: paul
 */
#include "stm32l5xx_hal.h"
#include "main.h"

#define WRAPPER
#include "hal_exti_wrapper_s.h"
#include "hal_exti_wrapper_ns.h"


#define LED_GREEN_GPIO_PIN GPIO_PIN_7
#define LED_GREEN_GPIO_PORT GPIOC



#define USER_BUTTON_GPIO_PIN GPIO_PIN_13

typedef void CMSE_NS_CALL (*hal_gpio_exti_callback_t)(uint16_t);

// can't link against NS, so NS wrapper sets this up at startup
volatile hal_gpio_exti_callback_t rising_c, falling_c;
void CMSE_NS_ENTRY NS_HAL_GPIO_EXTI_setup(void (*r)(uint16_t), void (*f)(uint16_t)) {
  rising_c = (hal_gpio_exti_callback_t) r;
  falling_c = (hal_gpio_exti_callback_t) f;
  for (int i = 0; i< 6; ++i) {
    HAL_GPIO_TogglePin(LED_GREEN_GPIO_PORT, LED_GREEN_GPIO_PIN);
    HAL_Delay(100);
  }
  for (int i = 0; i< 6; ++i) {
      falling_c(0);
      HAL_Delay(200);
   }
}

void __attribute__((weak)) S_HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {

}

void __attribute__((weak)) S_HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {

}

// setup to get hal working...
/*void EXTI13_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}*/

uint32_t down_time;

// hook the Wrapper into HAL
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
  S_HAL_GPIO_EXTI_Rising_Callback(GPIO_Pin);
  if (GPIO_Pin == USER_BUTTON_GPIO_PIN) {
    down_time = HAL_GetTick();
  }
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
  S_HAL_GPIO_EXTI_Falling_Callback(GPIO_Pin);
  switch (GPIO_Pin) {
  case USER_BUTTON_GPIO_PIN:
    if (down_time && (HAL_GetTick() - down_time) < 1000) {
      rising_c(USER_BUTTON_GPIO_PIN);
      falling_c(USER_BUTTON_GPIO_PIN);
    }
  }
}

void HAL_EXTI_WRAPPER_timer_callback() {

}

