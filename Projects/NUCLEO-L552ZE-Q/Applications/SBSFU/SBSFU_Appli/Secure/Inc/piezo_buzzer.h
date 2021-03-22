
#ifndef PIEZO_BUZZER_H
#define PIEZO_BUZZER_H

#include "stm32l5xx_hal.h"

#define PIEZO_BUZZER_GPIO_PORT GPIOA
#define PIEZO_BUZZER_GPIO_PIN GPIO_PIN_5

void piezoBuzzer_setup(void);

void piezoBuzzer_buzz(int high_ms, int low_ms, int count);

void piezoBuzzer_highBuzz(int high_spin, int low_spin, int count);

#endif
