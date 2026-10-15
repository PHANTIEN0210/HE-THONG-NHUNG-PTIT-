#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

typedef struct {
    uint32_t Pin;
    uint32_t Mode;
    uint32_t Pull;
    uint32_t Speed;
} GPIO_InitTypeDef;

typedef struct {
    uint32_t placeholder;
} GPIO_TypeDef;

#define GPIO_PIN_0    ((uint16_t)0x0001)
#define GPIO_PIN_1    ((uint16_t)0x0002)
#define GPIO_PIN_10   ((uint16_t)0x0400)

#define GPIOB         ((GPIO_TypeDef *)0)

#define GPIO_MODE_OUTPUT_PP  0x00000001
#define GPIO_NOPULL          0x00000000
#define GPIO_SPEED_FREQ_LOW  0x00000002

#define __HAL_RCC_GPIOB_CLK_ENABLE()   ((void)0)

void HAL_Init(void);
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init);

#endif /* MAIN_H */
