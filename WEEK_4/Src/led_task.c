#include "led_task.h"

#define GPIOB_ODR     (*(volatile uint32_t*)(0x40010C0C))
#define GPIOB_BSRR    (*(volatile uint32_t*)(0x40010C10))

void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 800; i++) {
        __asm__("nop");
    }
}

void vLED_Toggle(uint32_t pin_mask) {
    if (GPIOB_ODR & pin_mask) {
        GPIOB_BSRR = pin_mask << 16; /* Tắt LED (Reset pin) */
    } else {
        GPIOB_BSRR = pin_mask;       /* Bật LED (Set pin) */
    }
}
