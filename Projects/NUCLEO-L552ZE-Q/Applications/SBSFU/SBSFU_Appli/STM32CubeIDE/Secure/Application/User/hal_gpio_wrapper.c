/*
 * hal_gpio_wrapper.c
 *
 *  Created on: May 18, 2021
 *      Author: paul
 */

/* used for state indication */
#define LED_BLUE_GPIO_PIN GPIO_PIN_7
#define LED_BLUE_GPIO_PORT GPIOB

#include "stm32l5xx_hal.h"
#include "main.h"

#define WRAPPER
#include "hal_gpio_wrapper_s.h"
#include "hal_gpio_wrapper_ns.h"

#define SECURE_LOCK_LAG 10

unsigned locked_for_secure = 0;

#define INVERT_PINSTATE(P) \
   ((P) == GPIO_PIN_RESET ? GPIO_PIN_SET : GPIO_PIN_RESET)

GPIO_PinState secure_state;
GPIO_PinState nonsecure_state;

// the mechanism to detect fake alarms
static unsigned total_flips = 0;
static unsigned current_flip = 0;
static unsigned flip_counters[100] = {0};
// 0.9 * 1ms * 100%
static unsigned forbidden_min = 90;
// 1.1 * 1ms * 100%
static unsigned forbidden_max = 110;

static void ns_flip() {
  ++total_flips;
  ++flip_counters[current_flip];
  if (forbidden_min < total_flips && total_flips < forbidden_max)
    locked_for_secure = 1000;
}

static void flips_tick() {
  current_flip = (current_flip + 1) % (sizeof(flip_counters)/sizeof(flip_counters[0]));
  total_flips -= flip_counters[current_flip];
  flip_counters[current_flip] = 0;
}

CMSE_NS_ENTRY void NS_HAL_GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct) {
  // don't do anything. Either this reaches us erroneously or secure set up already.
  return;
}

CMSE_NS_ENTRY void NS_HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState) {
  if (GPIOx == GPIOA_NS && GPIO_Pin == GPIO_PIN_5) {
    if (PinState != nonsecure_state)
      ns_flip();
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
    nonsecure_state = INVERT_PINSTATE(nonsecure_state);
    ns_flip();
    if (locked_for_secure <= 0) {
      HAL_GPIO_TogglePin(GPIOA_S, GPIO_PIN_5);
    } else {
      //HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
    }
  } else {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
    // not regarding the correct pin. just ignore...
  }
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
  flips_tick();
  HAL_GPIO_WritePin(LED_BLUE_GPIO_PORT, LED_BLUE_GPIO_PIN, locked_for_secure ? GPIO_PIN_SET : GPIO_PIN_RESET);
  /*total_flips += flip_counters[current_flip];
  current_flip = (current_flip + 1) % 100;
  total_flips -= flip_counters[current_flip];
  flip_counters[current_flip] = 0;
  if (locked_for_secure == 0) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, nonsecure_state);
  }*/
}

