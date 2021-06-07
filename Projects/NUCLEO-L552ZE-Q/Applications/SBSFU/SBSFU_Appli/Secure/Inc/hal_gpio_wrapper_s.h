#ifndef HAL_GPIO_WRAPPER_S_H
#define HAL_GPIO_WRAPPER_S_H

void S_HAL_GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);

void S_HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);

void S_HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

void HAL_GPIO_WRAPPER_timer_callback();

#endif

#ifndef WRAPPER

#define HAL_GPIO_Init S_HAL_GPIO_Init

#define HAL_GPIO_WritePin S_HAL_GPIO_WritePin

#define HAL_GPIO_TogglePin S_HAL_GPIO_TogglePin

#endif
