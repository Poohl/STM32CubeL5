
#include "piezo_buzzer.h"
#include "exti_toggle.h"

#include "stm32l5xx_hal.h"

void piezoBuzzer_setup(void) {
  GPIO_OUTPUT_SETUP(PIEZO_BUZZER_GPIO_PORT, PIEZO_BUZZER_GPIO_PIN, GPIO_SPEED_FREQ_LOW, GPIO_PIN_SEC)
}

void piezoBuzzer_buzz(int high_ms, int low_ms, int count) {
  for (; count > 0; --count) {
    HAL_GPIO_WritePin(PIEZO_BUZZER_GPIO_PORT, PIEZO_BUZZER_GPIO_PIN, GPIO_PIN_SET);
    HAL_Delay(high_ms);
    HAL_GPIO_WritePin(PIEZO_BUZZER_GPIO_PORT, PIEZO_BUZZER_GPIO_PIN, GPIO_PIN_RESET);
    HAL_Delay(low_ms);
  }
}

void piezoBuzzer_highBuzz(int high_spin, int low_spin, int count) {
  for (; count > 0; --count) {
    HAL_GPIO_WritePin(PIEZO_BUZZER_GPIO_PORT, PIEZO_BUZZER_GPIO_PIN, GPIO_PIN_SET);
    for (int i = 0; i < high_spin; ++i) asm("mov r0,r0");
    HAL_GPIO_WritePin(PIEZO_BUZZER_GPIO_PORT, PIEZO_BUZZER_GPIO_PIN, GPIO_PIN_RESET);
    for (int i = 0; i < low_spin; ++i) asm("mov r0,r0");
  }
}
