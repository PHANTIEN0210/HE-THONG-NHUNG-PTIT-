#include "button.h"
#include "stm32f1xx.h"
#include "task.h"

static QueueHandle_t countQueue;
static uint32_t buttonCount;

void Button_EXTI_Init(QueueHandle_t queue)
{
    countQueue = queue;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

    /* PA5 input pull-up; nut noi GND, nhan muc 0. */
    GPIOA->BSRR = (1 << 5);
    GPIOA->CRL &= ~(0xF << 20);
    GPIOA->CRL |= (0x8 << 20);

    /* PC13 output push-pull 2 MHz, muc 1 tat LED. */
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    GPIOC->BSRR = (1 << 13);
    GPIOC->CRH &= ~(0xF << 20);
    GPIOC->CRH |= (0x2 << 20);

    /* EXTI5 lay tu PA5, chi canh xuong; chua cho phep ngat. */
    EXTI->IMR &= ~(1 << 5);
    AFIO->EXTICR[1] &= ~(0xF << 4);
    EXTI->RTSR &= ~(1 << 5);
    EXTI->FTSR |= (1 << 5);
    EXTI->PR = (1 << 5);

    /* Priority 12 thuoc mien 5..15 duoc goi API FreeRTOS tu ISR. */
    NVIC_SetPriority(EXTI9_5_IRQn, 12);
    NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
}

void Button_EXTI_Enable(void)
{
    EXTI->PR = (1 << 5);
    NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    EXTI->IMR |= (1 << 5);
}

void Led_Blink(void)
{
    /* Chi goi tu task: PC13 sang muc 0 trong khoang 100 ms. */
    GPIOC->BRR = (1 << 13);
    vTaskDelay(pdMS_TO_TICKS(100));
    GPIOC->BSRR = (1 << 13);
}

void Button_WaitRelease(void)
{
    for (;;) {
        if ((GPIOA->IDR & (1 << 5)) != 0) {
            /* PR van ghi nhan canh xuong khi IMR dang khoa. */
            EXTI->PR = (1 << 5);
            vTaskDelay(pdMS_TO_TICKS(50));

            /* Chi mo lai khi tha nut va khong co canh xuong trong 50 ms. */
            if ((GPIOA->IDR & (1 << 5)) != 0 &&
                (EXTI->PR & (1 << 5)) == 0) {
                EXTI->IMR |= (1 << 5);
                return;
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

void EXTI9_5_IRQHandler(void)
{
    BaseType_t higherPriorityTaskWoken = pdFALSE;

    if ((EXTI->PR & (1 << 5)) != 0 && (EXTI->IMR & (1 << 5)) != 0) {
        /* Khoa EXTI5 den khi tha nut, tranh dem lap do doi hoac giu nut. */
        EXTI->IMR &= ~(1 << 5);
        EXTI->PR = (1 << 5);
        ++buttonCount;

        /* Moi lan mo ngat chi gui mot gia tri; task nhan xong moi mo lai. */
        BaseType_t result = xQueueSendFromISR(countQueue, &buttonCount,
                                            &higherPriorityTaskWoken);
        configASSERT(result == pdPASS);
    }

    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}
