
#ifndef NSW_HAL_GPIO_WRAPPER_H
#define NSW_HAL_GPIO_WRAPPER_H

#include "hal_gpio_wrapper_ns.h"

void NSW_HAL_GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);

void NSW_HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);

void NSW_HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

#define WRAPPER
#ifndef WRAPPER

#define HAL_GPIO_Init NSW_HAL_GPIO_Init

#define HAL_GPIO_WritePin NSW_HAL_GPIO_WritePin

#define HAL_GPIO_TogglePin NSW_HAL_GPIO_TogglePin

#undef WRAPPER
#endif

#endif
