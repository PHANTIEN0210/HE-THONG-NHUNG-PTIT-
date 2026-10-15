#include <stdint.h>
#include "led_task.h"

#define RCC_APB2ENR   (*(volatile uint32_t*)(0x40021018))
#define GPIOB_CRL     (*(volatile uint32_t*)(0x40010C00))
#define GPIOB_CRH     (*(volatile uint32_t*)(0x40010C04))

int main(void) {
    /* 1. Bật xung nhịp GPIOB */
    RCC_APB2ENR |= (1 << 3);

    /* 2. Cấu hình PB0, PB1 là Output Push-Pull 50MHz (CRL) */
    GPIOB_CRL &= ~((0xF << 0) | (0xF << 4));
    GPIOB_CRL |=  ((0x3 << 0) | (0x3 << 4));

    /* 3. Cấu hình PB10 là Output Push-Pull 50MHz (CRH) */
    GPIOB_CRH &= ~(0xF << 8);
    GPIOB_CRH |=  (0x3 << 8);

    uint32_t counter = 0;

    while (1) {
        /* Task PB10: 10Hz (50ms) */
        vLED_Toggle(1 << 10);
        delay_ms(50);

        counter++;

        /* Task PB1: 1Hz (500ms) */
        if (counter % 10 == 0) {
            vLED_Toggle(1 << 1);
        }

        /* Task PB0: 0.1Hz (5000ms) */
        if (counter >= 100) {
            vLED_Toggle(1 << 0);
            counter = 0;
        }
    }
}
