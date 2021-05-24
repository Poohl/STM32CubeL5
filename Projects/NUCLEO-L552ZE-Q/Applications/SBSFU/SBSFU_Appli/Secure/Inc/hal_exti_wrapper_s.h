#ifndef HAL_EXTI_WRAPPER_S_H
#define HAL_EXTI_WRAPPER_S_H

void S_HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin);

void S_HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin);

void HAL_EXTI_WRAPPER_timer_callback();

#define WRAPPER
#ifndef WRAPPER

#define HAL_GPIO_EXTI_Falling_Callback S_HAL_GPIO_EXTI_Falling_Callback

#define HAL_GPIO_EXTI_Rising_Callback S_HAL_GPIO_EXTI_Rising_Callback

#undef WRAPPER
#define

#endif
#endif
