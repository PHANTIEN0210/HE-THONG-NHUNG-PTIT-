#ifndef BUTTON_H
#define BUTTON_H

#include "FreeRTOS.h"
#include "queue.h"

void Button_EXTI_Init(QueueHandle_t queue);
/* Goi tu task sau khi scheduler da chay. */
void Button_EXTI_Enable(void);
/* Nhay PC13 100 ms; chi goi tu task. */
void Led_Blink(void);
/* Goi sau khi nhan queue; cho tha nut on dinh roi mo lai EXTI5. */
void Button_WaitRelease(void);

#endif
