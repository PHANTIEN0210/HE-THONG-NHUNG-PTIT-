#include "stm32f1xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "led_button.h"

QueueHandle_t qLED1, qLED2, qLED3;

void vTaskButton(void *pvParameters) {
    uint8_t current_led = 1; 
    uint16_t brightness[3] = {0, 0, 0}; 
    
    while(1) {
        if (ReadButton(1) == 0) { 
            current_led++;
            if (current_led > 3) current_led = 1;
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        
        if (ReadButton(2) == 0) {
            if (brightness[current_led - 1] < 1000) brightness[current_led - 1] += 100; 
            if (current_led == 1) xQueueSend(qLED1, &brightness[0], 0);
            else if (current_led == 2) xQueueSend(qLED2, &brightness[1], 0);
            else if (current_led == 3) xQueueSend(qLED3, &brightness[2], 0);
            vTaskDelay(pdMS_TO_TICKS(200)); 
        }
        
        if (ReadButton(3) == 0) {
            if (brightness[current_led - 1] >= 100) brightness[current_led - 1] -= 100; 
            if (current_led == 1) xQueueSend(qLED1, &brightness[0], 0);
            else if (current_led == 2) xQueueSend(qLED2, &brightness[1], 0);
            else if (current_led == 3) xQueueSend(qLED3, &brightness[2], 0);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void vTaskLED1(void *pvParameters) {
    uint16_t target;
    while(1) {
        if (xQueueReceive(qLED1, &target, portMAX_DELAY) == pdPASS) Set_PWM_Duty(1, target);
    }
}

void vTaskLED2(void *pvParameters) {
    uint16_t target;
    while(1) {
        if (xQueueReceive(qLED2, &target, portMAX_DELAY) == pdPASS) Set_PWM_Duty(2, target);
    }
}

void vTaskLED3(void *pvParameters) {
    uint16_t target;
    while(1) {
        if (xQueueReceive(qLED3, &target, portMAX_DELAY) == pdPASS) Set_PWM_Duty(3, target);
    }
}

int main(void) {
    GPIO_Init();
    PWM_Init_1kHz();
    Set_PWM_Duty(1, 0);
    Set_PWM_Duty(2, 0);
    Set_PWM_Duty(3, 0);
    
    qLED1 = xQueueCreate(1, sizeof(uint16_t));
    qLED2 = xQueueCreate(1, sizeof(uint16_t));
    qLED3 = xQueueCreate(1, sizeof(uint16_t));
    
    xTaskCreate(vTaskButton, "TaskBtn", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(vTaskLED1,   "TaskL1",  configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(vTaskLED2,   "TaskL2",  configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(vTaskLED3,   "TaskL3",  configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    
    vTaskStartScheduler();
    while(1) {}
}
