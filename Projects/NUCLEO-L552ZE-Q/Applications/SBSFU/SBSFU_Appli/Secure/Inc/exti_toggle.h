
#ifndef EXTI_TOGGLE_H
#define EXTI_TOGGLE_H

#include "stm32l5xx_hal.h"

/*
 * Macros to do the code generation usually done by the ... code generator.
 */

/*
 * You have to love GCC for this shit, otherwise you go mad.
 *
 * Any argument is expanded before the macro itself is expanded,
 * except for when the argument is used as argument to ## or #.
 * Since X and Y are not used as arguments to ## or # in EVAL_CONCAT
 * they are expanded. Then in CONCAT they are not expanded but
 * just pasted together.
 * The result is treated as new token and expanded once more.
 *
 * The Macro-Expansion-preview is having none of this.
 */
#define CONCAT(X,Y) X ## Y
#define EVAL_CONCAT(X,Y) CONCAT(X,Y)

// Because EXTI is defined somewhere, we need a version passing it implicitly
#define PREFIX_EXTI(X) EXTI ## X
#define EVAL_PREFIX_EXTI(X) PREFIX_EXTI(X)

#define PIN_TO_GPIO_PIN(PIN) EVAL_CONCAT(GPIO_PIN_, PIN)
#define PORT_TO_GPIO_PORT(PORT) EVAL_CONCAT(GPIO, PORT)
#define PIN_TO_EXTI_LINE(PIN) EVAL_CONCAT(EXTI_LINE_, PIN)
#define PIN_TO_EXTI_IRQ(PIN) EVAL_CONCAT(EVAL_PREFIX_EXTI(PIN),_IRQn)
#define PIN_TO_EXTI_IRQ_HANDLER(PIN) EVAL_CONCAT(PREFIX_EXTI(PIN),_IRQHandler)


/**
 * Sets up a Pin for exti interrupts
 *
 * PORT: macro or literal A-G
 * PIN: macro or literal 1-16
 * WORLD: macro or literal SEC or NSEC
 * PRIO: macro or literal 0-7
 * PREEMPT_PRIO: macro or literal 0-7
 */
#define EXTI_TOGGLE_SETUP(PORT, PIN, WORLD, PRIO, PREEMPT_PRIO) \
  { \
    GPIO_InitTypeDef GPIO_InitStruct = {0}; \
    HAL_EXTI_ConfigLineAttributes(PIN_TO_EXTI_LINE(PIN), EVAL_CONCAT(EXTI_LINE_, WORLD)); \
    GPIO_InitStruct.Pin = PIN_TO_GPIO_PIN(PIN); \
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING; \
    GPIO_InitStruct.Pull = GPIO_NOPULL; \
    HAL_GPIO_Init(PORT_TO_GPIO_PORT(PORT), &GPIO_InitStruct); \
    HAL_NVIC_SetPriority(PIN_TO_EXTI_IRQ(PIN), PRIO, PREEMPT_PRIO); \
    HAL_NVIC_EnableIRQ(PIN_TO_EXTI_IRQ(PIN)); \
    HAL_GPIO_ConfigPinAttributes(PORT_TO_GPIO_PORT(PORT), PIN_TO_GPIO_PIN(PIN), EVAL_CONCAT(GPIO_PIN_, WORLD)); \
  }

#define EXTI_TOGGLE_IS_HIGH(PORT, PIN) \
    (GPIO_PIN_SET == HAL_GPIO_ReadPin(PORT_TO_GPIO_PORT(PORT), PIN_TO_GPIO_PIN(PIN)))

#define EXTI_TOGGLE_IRQ_HANDLER(PIN, RISING, FALLING) \
  void PIN_TO_EXTI_IRQ_HANDLER(PIN) (void) { \
    if(__HAL_GPIO_EXTI_GET_RISING_IT(PIN_TO_GPIO_PIN(PIN)) != 0U) { \
      __HAL_GPIO_EXTI_CLEAR_RISING_IT(PIN_TO_GPIO_PIN(PIN)); \
      RISING(); \
    } \
    if(__HAL_GPIO_EXTI_GET_FALLING_IT(PIN_TO_GPIO_PIN(PIN)) != 0U) { \
      __HAL_GPIO_EXTI_CLEAR_FALLING_IT(PIN_TO_GPIO_PIN(PIN)); \
      FALLING(); \
    } \
  }

#define GPIO_OUTPUT_SETUP(GPIO_PORT, GPIO_PIN, GPIO_SPEED, GPIO_WORLD) \
    { \
      GPIO_InitTypeDef GPIO_InitStruct = {0}; \
      GPIO_InitStruct.Pin = GPIO_PIN; \
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; \
      GPIO_InitStruct.Pull = GPIO_NOPULL; \
      GPIO_InitStruct.Speed = GPIO_SPEED; \
      HAL_GPIO_Init(GPIO_PORT, &GPIO_InitStruct); \
      HAL_GPIO_ConfigPinAttributes(GPIO_PORT, GPIO_PIN, GPIO_WORLD); \
    }

#endif
