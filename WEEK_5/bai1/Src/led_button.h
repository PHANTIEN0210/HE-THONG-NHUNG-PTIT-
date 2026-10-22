#ifndef LED_BUTTON_H
#define LED_BUTTON_H

#include "stm32f1xx.h"

void GPIO_Init(void);
void PWM_Init_1kHz(void);
void Set_PWM_Duty(uint8_t led_index, uint16_t duty);
uint8_t ReadButton(uint8_t btn_index);

#endif /* LED_BUTTON_H */
