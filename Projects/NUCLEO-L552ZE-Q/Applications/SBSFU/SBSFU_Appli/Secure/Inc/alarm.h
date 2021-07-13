
#ifndef ALARM_H
#define ALARM_H

// one might think this is the exact shit the HAL handles
// one might think this is somewhere in the documentation
// one would be mistaken
// one is advised to reverse engineer an example to figure this out
// User button: logical: C13, physical: Blue button
#define USER_BUTTON_GPIO_PORT GPIOC
#define USER_BUTTON_GPIO_PIN GPIO_PIN_13
#define USER_BUTTON_EXTI_LINE EXTI_LINE_13
#define USER_BUTTON_IRQn EXTI13_IRQn
#define USER_BUTTON_IRQ_HANDLER EXTI13_IRQHandler

// Window sensor: logical A6, physical: D12
#define WINDOW_SENSOR_GPIO_PIN GPIO_PIN_6
#define WINDOW_SENSOR_GPIO_PORT GPIOA
#define WINDOW_SENSOR_EXTI_LINE EXTI_LINE_6
#define WINDOW_SENSOR_IRQn EXTI6_IRQn
#define WINDOW_SENSOR_IRQ_HANDLER EXTI6_IRQHandler

// Piezo buzzer: logical A5, physical D13
#define PIEZO_BUZZER_GPIO_PORT GPIOA
#define PIEZO_BUZZER_GPIO_PIN GPIO_PIN_5

#define LED_RED_GPIO_PIN GPIO_PIN_9
#define LED_RED_GPIO_PORT GPIOA

#define LED_GREEN_GPIO_PIN GPIO_PIN_7
#define LED_GREEN_GPIO_PORT GPIOC

int alarm_setup(void);

#endif
