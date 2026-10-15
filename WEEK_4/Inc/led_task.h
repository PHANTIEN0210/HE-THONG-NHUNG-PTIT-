#ifndef LED_TASK_H
#define LED_TASK_H

#include <stdint.h>

void delay_ms(uint32_t ms);
void vLED_Toggle(uint32_t pin_mask);

#endif
