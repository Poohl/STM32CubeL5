
#ifndef HAL_SYSTICK_WRAPPER_H
#define HAL_SYSTICK_WRAPPER_H

void HAL_SYSTICK_Callback_A(void);

#endif

#ifndef WRAPPER

#define HAL_SYSTICK_Callback HAL_SYSTICK_Callback_A

#endif
