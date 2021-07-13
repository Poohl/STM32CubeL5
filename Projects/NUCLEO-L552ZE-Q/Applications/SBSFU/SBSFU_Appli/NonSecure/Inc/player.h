#ifndef PLAYER_H
#define PLAYER_H

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

// Piezo buzzer: logical A5, physical D13
#define PIEZO_BUZZER_GPIO_PORT GPIOA
#define PIEZO_BUZZER_GPIO_PIN GPIO_PIN_5

#define LED_GREEN_GPIO_PIN GPIO_PIN_7
#define LED_GREEN_GPIO_PORT GPIOC

void player_setup(void);

void player_play(void);

#endif
