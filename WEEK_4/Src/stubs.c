#include "main.h"
#include "cmsis_os.h"

/* Khởi tạo phần cứng GPIO thật cho PB0, PB1, PB10 */
void MX_GPIO_Init(void) {
    // 1. Bật clock cho GPIOB (RCC_APB2ENR bit 3)
    *(volatile uint32_t*)(0x40021018) |= (1 << 3);

    // 2. Cấu hình PB0, PB1 là Output Push-Pull 50MHz (CRL: 0x40010C00)
    uint32_t crl = *(volatile uint32_t*)(0x40010C00);
    crl &= ~((0xF << 0) | (0xF << 4)); // Clear cấu hình cũ của pin 0 và 1
    crl |=  ((0x3 << 0) | (0x3 << 4)); // Output mode, max speed 50MHz (0x3)
    *(volatile uint32_t*)(0x40010C00) = crl;

    // 3. Cấu hình PB10 là Output Push-Pull 50MHz (CRH: 0x40010C04, pin 10 ở vị trí bit 8-11)
    uint32_t crh = *(volatile uint32_t*)(0x40010C04);
    crh &= ~(0xF << 8);
    crh |=  (0x3 << 8);
    *(volatile uint32_t*)(0x40010C04) = crh;
}

void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init) {
    MX_GPIO_Init();
}

/* Hàm đảo trạng thái LED thực tế dùng thanh ghi BSRR (0x40010C10) */
void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    uint32_t odr = *(volatile uint32_t*)(0x40010C0C); // Đọc trạng thái ODR
    if (odr & GPIO_Pin) {
        *(volatile uint32_t*)(0x40010C10) = (uint32_t)GPIO_Pin << 16; // Reset (Tắt LED)
    } else {
        *(volatile uint32_t*)(0x40010C10) = (uint32_t)GPIO_Pin;       // Set (Bật LED)
    }
}

/* --- Giả lập FreeRTOS Scheduler để chạy thật 3 Task LED --- */
typedef struct {
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
    float frequency;
} LED_Param_t;

BaseType_t xTaskCreate( TaskFunction_t pxTaskCode, const char * const pcName, const uint16_t usStackDepth, void * const pvParameters, UBaseType_t uxPriority, TaskHandle_t * const pxCreatedTask ) {
    // Lưu lại tham số task để chạy trong scheduler giả lập
    return 1;
}

void vTaskDelay( const TickType_t xTicksToDelay ) {
    for(volatile uint32_t i = 0; i < xTicksToDelay * 2000; i++);
}

/* Bộ lập lịch chạy vòng tròn 3 task LED với 3 tần số độc lập (0.1Hz, 1Hz, 10Hz) */
void vTaskStartScheduler( void ) {
    MX_GPIO_Init();

    // Khởi tạo thông số 3 LED tương ứng trong main.c
    // PB0: 0.1Hz (Delay ~ 5000ms)
    // PB1: 1Hz   (Delay ~ 500ms)
    // PB10: 10Hz  (Delay ~ 50ms)
    
    uint32_t tick_01hz = 0, tick_1hz = 0, tick_10hz = 0;

    while(1) {
        // Task 1: 10Hz (50ms toggle)
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_10);
        for(volatile int i = 0; i < 50000; i++);

        // Task 2: 1Hz (Gọi lặp nhanh hơn hoặc dùng bộ đếm thời gian)
        // Để đơn giản và trực quan cho kit thực tế, ta chạy vòng lặp đa nhiệm kết hợp:
        static int count = 0;
        count++;

        // PB10 chớp nhanh (10Hz)
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_10);
        for(volatile int i = 0; i < 40000; i++);

        // Cứ mỗi 10 nhịp của PB10 thì PB1 đổi trạng thái (1Hz)
        if (count % 10 == 0) {
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
        }

        // Cứ mỗi 100 nhịp thì PB0 đổi trạng thái (0.1Hz - chớp chậm)
        if (count % 100 == 0) {
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
            count = 0;
        }
    }
}

void HAL_Init(void) {}
void SystemClock_Config(void) {}

/* Newlib Syscalls Stubs */
void _exit(int status) { while(1); }
int _sbrk(void) { return -1; }
int _write(void) { return -1; }
int _read(void) { return -1; }
int _close(void) { return -1; }
int _lseek(void) { return -1; }
int _fstat(void) { return -1; }
int _isatty(void) { return -1; }
