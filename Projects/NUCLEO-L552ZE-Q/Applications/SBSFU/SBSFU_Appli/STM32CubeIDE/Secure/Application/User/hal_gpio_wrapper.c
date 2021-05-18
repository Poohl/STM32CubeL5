/*
 * hal_gpio_wrapper.c
 *
 *  Created on: May 18, 2021
 *      Author: paul
 */



#include "stm32l5xx_hal.h"
#include "main.h"

#include "hal_gpio_wrapper_s.h"
#include "hal_gpio_wrapper_ns.h"

#define SECURE_LOCK_LAG 10

char locked_for_secure = 0;

GPIO_PinState secure_state;
GPIO_PinState nonsecure_state;

/*
int total_flips = 0;
unsigned current_flip = 0;
int flip_counters[100] = {0};
*/
CMSE_NS_ENTRY void NS_HAL_GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct) {
  // don't do anything. Either this reaches us erroneously or secure set up already.
  return;
}

CMSE_NS_ENTRY void NS_HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState) {
  if (GPIOx == GPIOA_NS && GPIO_Pin == GPIO_PIN_5) {
    nonsecure_state = PinState;
    if (locked_for_secure <= 0) {
      HAL_GPIO_WritePin(GPIOA_S, GPIO_PIN_5, PinState);
    }
  } else {
    // not regarding the correct pin. just ignore...
  }
}

CMSE_NS_ENTRY void NS_HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
  if (GPIOx == GPIOA_NS && GPIO_Pin == GPIO_PIN_5) {
    nonsecure_state = !nonsecure_state;
    if (locked_for_secure <= 0) {
      HAL_GPIO_TogglePin(GPIOA_S, GPIO_PIN_5);
    } else {
      //HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
    }
  } else {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
    // not regarding the correct pin. just ignore...
  }
  //HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

void S_HAL_GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct) {
  HAL_GPIO_Init(GPIOx, GPIO_InitStruct);
}

void S_HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState) {

  if (GPIOx == GPIOA && GPIO_Pin == GPIO_PIN_5) {

    if (!locked_for_secure) {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, secure_state);
     }
    secure_state = PinState;
    locked_for_secure = SECURE_LOCK_LAG;
  }
  HAL_GPIO_WritePin(GPIOx, GPIO_Pin, PinState);
}

void S_HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
  if (GPIOx == GPIOA && GPIO_Pin == GPIO_PIN_5) {
    if (!locked_for_secure) {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, secure_state);
    }
    secure_state = !secure_state;
    locked_for_secure = SECURE_LOCK_LAG;
  }
  HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);
}


void HAL_GPIO_WRAPPER_timer_callback() {
  if (locked_for_secure > 0) {
    --locked_for_secure;
  }
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, locked_for_secure ? GPIO_PIN_SET : GPIO_PIN_RESET);
  /*total_flips += flip_counters[current_flip];
  current_flip = (current_flip + 1) % 100;
  total_flips -= flip_counters[current_flip];
  flip_counters[current_flip] = 0;
  if (locked_for_secure == 0) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, nonsecure_state);
  }*/
}

