

#define WRAPPER

#include "hal_gpio_wrapper_ns.h"
#include "nsw_hal_gpio_wrapper.h"

#define PIEZO_BUZZER_GPIO_PORT GPIOA
#define PIEZO_BUZZER_GPIO_PIN GPIO_PIN_5

void NSW_HAL_GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct) {
  // Dont init the piezo buzzer,
  if (GPIOx == PIEZO_BUZZER_GPIO_PORT)
    GPIO_InitStruct->Pin &= ~PIEZO_BUZZER_GPIO_PIN;
  HAL_GPIO_Init(GPIOx, GPIO_InitStruct);
}

void NSW_HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState) {
  if (GPIOx == PIEZO_BUZZER_GPIO_PORT && GPIO_Pin == PIEZO_BUZZER_GPIO_PIN) {
    NS_HAL_GPIO_WritePin(GPIOx, GPIO_Pin, PinState);
  } else {
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, PinState);
  }
}

void NSW_HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
  if (GPIOx == PIEZO_BUZZER_GPIO_PORT && GPIO_Pin == PIEZO_BUZZER_GPIO_PIN) {
      NS_HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);
    } else {
      HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);
    }
}
