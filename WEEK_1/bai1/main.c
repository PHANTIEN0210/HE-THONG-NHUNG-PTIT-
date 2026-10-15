#define RCC_APB2ENR (*(volatile unsigned int *)0x40021018)
#define GPIOC_CRH   (*(volatile unsigned int *)0x40011004)
#define GPIOC_ODR   (*(volatile unsigned int *)0x4001100C)

void delay(volatile unsigned int count) {
    while(count--) {}
}

int main(void) {
    RCC_APB2ENR |= (1 << 4); 
    GPIOC_CRH &= ~(0xF << 20);
    GPIOC_CRH |= (0x2 << 20);  
    
    while(1) {
        GPIOC_ODR ^= (1 << 13);
        delay(1000000);         
    }
}
