#ifndef HAL_GPIO_WRAPPER_NS_H
#define HAL_GPIO_WRAPPER_NS_H

#include "stm32l5xx_hal.h"

void NS_HAL_GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);

void NS_HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);

void NS_HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

#endif
