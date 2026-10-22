#include "uart.h"
#include "stm32f1xx.h"

void UART1_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;

    /* PA9: TX alternate-function push-pull, 50 MHz. */
    GPIOA->CRH &= ~(0xF << 4);
    GPIOA->CRH |= (0xB << 4);

    /* PCLK2 = HCLK = 72 MHz; USART1 115200 baud, 8N1. */
    USART1->BRR = (SystemCoreClock + 115200 / 2) / 115200;
    USART1->CR1 = USART_CR1_TE | USART_CR1_UE;
}

static void UART1_SendChar(char character)
{
    while ((USART1->SR & USART_SR_TXE) == 0) {
    }
    USART1->DR = (uint16_t)character;
}

void UART1_SendText(const char *text)
{
    while (*text != '\0') {
        UART1_SendChar(*text++);
    }
}

void UART1_SendUnsigned(uint32_t value)
{
    char digits[10];
    uint32_t length = 0;

    /* Tach chu so tu phai sang trai, ke ca gia tri 0. */
    do {
        digits[length++] = (char)('0' + (value % 10));
        value /= 10;
    } while (value != 0);

    /* Gui nguoc mang de hien thi dung thu tu thap phan. */
    while (length != 0) {
        UART1_SendChar(digits[--length]);
    }
}
