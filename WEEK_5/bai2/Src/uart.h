#ifndef UART_H
#define UART_H

#include <stdint.h>

void UART1_Init(void);
void UART1_SendText(const char *text);
void UART1_SendUnsigned(uint32_t value);

#endif
