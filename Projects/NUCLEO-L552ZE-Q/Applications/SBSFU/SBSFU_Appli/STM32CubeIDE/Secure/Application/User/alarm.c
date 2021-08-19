
#include "alarm.h"

#include "stm32l5xx_hal.h"
#include "hal_gpio_wrapper_s.h"
#include "hal_exti_wrapper_s.h"
#include "hal_systick_wrapper.h"

static volatile uint32_t btn_down_start = 0;
static volatile char alarm_level = 0;

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
  switch (GPIO_Pin) {
  case USER_BUTTON_GPIO_PIN:
        btn_down_start = 0;
        break;
  case WINDOW_SENSOR_GPIO_PIN:
    if (alarm_level >= 1)
       alarm_level += 1;
    break;
  default:
    break;
  }
  return;
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
  switch (GPIO_Pin) {
    case USER_BUTTON_GPIO_PIN:
      btn_down_start = HAL_GetTick();
      break;
    case WINDOW_SENSOR_GPIO_PIN:
      if (alarm_level >= 1)
         alarm_level += 1;
      break;
    default:
      break;
  }
}

void EXTI13_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void EXTI6_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
}

void HAL_SYSTICK_Callback(void) {
  if (alarm_level >= 2) {
    HAL_GPIO_TogglePin(PIEZO_BUZZER_GPIO_PORT, PIEZO_BUZZER_GPIO_PIN);
  }

  if (btn_down_start && (HAL_GetTick() - btn_down_start > 1000)) {
    btn_down_start = 0;
    alarm_level = !alarm_level;
  }
  HAL_GPIO_WritePin(LED_RED_GPIO_PORT, LED_RED_GPIO_PIN, alarm_level ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

int alarm_setup(void) {
  { // User Button
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = USER_BUTTON_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(USER_BUTTON_GPIO_PORT, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(USER_BUTTON_IRQn, 2, 2);
    HAL_NVIC_EnableIRQ(USER_BUTTON_IRQn);
  }

  { // Window Sensor
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = WINDOW_SENSOR_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(WINDOW_SENSOR_GPIO_PORT, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(WINDOW_SENSOR_IRQn, 2, 2);
    HAL_NVIC_EnableIRQ(WINDOW_SENSOR_IRQn);
  }

  { // red LED
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LED_RED_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_RED_GPIO_PORT, &GPIO_InitStruct);
  }

  { // Piezo Buzzer
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = PIEZO_BUZZER_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(PIEZO_BUZZER_GPIO_PORT, &GPIO_InitStruct);
  }

  return 0;
}

