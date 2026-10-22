#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "uart.h"
#include "button.h"

#define COUNT_QUEUE_LENGTH 16U

static QueueHandle_t countQueue;

static void UART_Task(void *argument)
{
    uint32_t count;

    (void)argument;

    /* Chi mo EXTI5 sau khi scheduler da chay. */
    Button_EXTI_Enable();

    for (;;)
    {
        /* Blocked cho ISR gui so dem moi. */
        if (xQueueReceive(countQueue, &count, portMAX_DELAY) == pdPASS)
        {
            UART1_SendText("So lan nhan nut: ");
            UART1_SendUnsigned(count);
            UART1_SendText("\r\n");
            Led_Blink();
            Button_WaitRelease();
        }
    }
}

int main(void)
{
    UART1_Init();

    countQueue = xQueueCreate(COUNT_QUEUE_LENGTH, sizeof(uint32_t));
    xTaskCreate(UART_Task, "UART", 256, NULL, 2, NULL);

    Button_EXTI_Init(countQueue);
    vTaskStartScheduler();

    for (;;)
    {
    }
}
