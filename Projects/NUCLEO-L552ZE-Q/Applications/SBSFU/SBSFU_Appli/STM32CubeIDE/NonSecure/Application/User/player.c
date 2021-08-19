
#include "player.h"

#include "stm32l5xx_hal.h"
#include "nsw_hal_gpio_wrapper.h"

static unsigned char track_0[1] = {0xFF}; // silence
static unsigned char track_1[1] = {4};
static unsigned char track_2[5] = {1, 1, 1, 1, 4};
static unsigned char track_3[2] = {2,1};
static unsigned char track_4[3] = {2};
static unsigned char track_5[7] = {1,1,1,1,1,1,2};

static unsigned char* tracks[6] = {&track_0[0], &track_1[0], &track_2[0], &track_3[0], &track_4[0], &track_5[0]};
static unsigned char track_len[6] = {1,1,5,2,3,7};

volatile static unsigned int track = 1;

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
  track = ((track + 1) % 6);
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
}

void EXTI13_IRQHandler(void) { // this doesn't do anything anymore. Interrupt targets Secure.
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
  uint32_t until = HAL_GetTick();
  int j = 0;
  for (int i = 0; i < 10000; i+= *(tracks[track])) {
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    until += tracks[track][(++j) % track_len[track]];
    while (HAL_GetTick() < until);
  }
}
