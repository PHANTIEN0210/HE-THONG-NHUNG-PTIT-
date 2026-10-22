#include "led_button.h"

void GPIO_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // Cấu hình PA0, PA1, PA2 (Output 50MHz, Alt Function Push-Pull)
    GPIOA->CRL &= ~(0x00000FFF); 
    GPIOA->CRL |= 0x00000BBB;

    // Cấu hình PA8, PA9, PA10 (Input Pull-up/Pull-down)
    GPIOA->CRH &= ~(0x00000FFF);
    GPIOA->CRH |= 0x00000888;
    
    // Bật Pull-up cho PA8, PA9, PA10
    GPIOA->ODR |= (1 << 8) | (1 << 9) | (1 << 10);
}

void PWM_Init_1kHz(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // SYSCLK = 72MHz, băm xung 1kHz
    TIM2->PSC = 72 - 1;       
    TIM2->ARR = 1000 - 1;

    // PWM Mode 1 cho CH1 (PA0), CH2 (PA1), CH3 (PA2)
    TIM2->CCMR1 |= (6 << 4) | TIM_CCMR1_OC1PE;  
    TIM2->CCMR1 |= (6 << 12) | TIM_CCMR1_OC2PE; 
    TIM2->CCMR2 |= (6 << 4) | TIM_CCMR2_OC3PE;  

    TIM2->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E;
    TIM2->CR1 |= TIM_CR1_CEN;
}

void Set_PWM_Duty(uint8_t led_index, uint16_t duty) {
    if (led_index == 1) TIM2->CCR1 = duty;       
    else if (led_index == 2) TIM2->CCR2 = duty;  
    else if (led_index == 3) TIM2->CCR3 = duty;  
}

uint8_t ReadButton(uint8_t btn_index) {
    if (btn_index == 1) return (GPIOA->IDR & (1 << 8))  ? 1 : 0; 
    if (btn_index == 2) return (GPIOA->IDR & (1 << 9))  ? 1 : 0; 
    if (btn_index == 3) return (GPIOA->IDR & (1 << 10)) ? 1 : 0; 
    return 1;
}
