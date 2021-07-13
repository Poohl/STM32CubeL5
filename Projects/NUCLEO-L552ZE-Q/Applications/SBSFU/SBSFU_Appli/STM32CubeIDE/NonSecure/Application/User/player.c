
#include "player.h"

#include "stm32l5xx_hal.h"
#include "nsw_hal_gpio_wrapper.h"



volatile static unsigned int track = 1;

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
  track = ((track) % 8) + 1;
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
}

void EXTI13_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void player_setup(void) {
  { // User Button
      GPIO_InitTypeDef GPIO_InitStruct = {0};
      GPIO_InitStruct.Pin = USER_BUTTON_GPIO_PIN;
      GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      HAL_GPIO_Init(USER_BUTTON_GPIO_PORT, &GPIO_InitStruct);
      HAL_NVIC_SetPriority(USER_BUTTON_IRQn, 2, 2);
      HAL_NVIC_EnableIRQ(USER_BUTTON_IRQn);
    }

    { // Piezo Buzzer
      GPIO_InitTypeDef GPIO_InitStruct = {0};
      GPIO_InitStruct.Pin = PIEZO_BUZZER_GPIO_PIN;
      GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      HAL_GPIO_Init(PIEZO_BUZZER_GPIO_PORT, &GPIO_InitStruct);
    }
}

void player_play(void) {
  for (int i = 0; i < 10000; i += track) {
    HAL_Delay(track);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
  }
}
