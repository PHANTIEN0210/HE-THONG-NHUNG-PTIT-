#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "stm32f1xx.h"

/* Clock CPU lay tu cau hinh he thong; tick moi 1 ms. */
#define configCPU_CLOCK_HZ                       (SystemCoreClock)
#define configTICK_RATE_HZ                       1000
#define configTICK_TYPE_WIDTH_IN_BITS            TICK_TYPE_WIDTH_32_BITS
#define configUSE_PREEMPTION                     1
#define configUSE_TIME_SLICING                   1
#define configMAX_PRIORITIES                     5
#define configMAX_TASK_NAME_LEN                  16
#define configIDLE_SHOULD_YIELD                  1

/* Stack tinh bang word 32 bit: 128 word = 512 byte. */
#define configMINIMAL_STACK_SIZE                128
#define configSUPPORT_STATIC_ALLOCATION         0
#define configSUPPORT_DYNAMIC_ALLOCATION        1
/* Heap khoi dau 8 KiB tren SRAM 20 KiB; dung voi heap_4.c. */
#define configTOTAL_HEAP_SIZE                    (8U * 1024U)

/* Khong yeu cau callback ung dung hay thu vien C reentrant. */
#define configUSE_IDLE_HOOK                      0
#define configUSE_TICK_HOOK                      0
#define configUSE_MALLOC_FAILED_HOOK             0
#define configCHECK_FOR_STACK_OVERFLOW           0
#define configUSE_NEWLIB_REENTRANT               0
#define configUSE_TIMERS                         0
#define configUSE_CO_ROUTINES                    0
#define configUSE_MUTEXES                        1

#define INCLUDE_vTaskDelay                       1
#define INCLUDE_xTaskDelayUntil                  1
#define INCLUDE_vTaskSuspend                     1
#define INCLUDE_vTaskDelete                      1

/* CMSIS dung priority chua dich; port FreeRTOS dung gia tri da dich.
 * Chi ISR co priority 5..15 duoc goi API FromISR (0 la cao nhat).
 * Khi tich hop, dung priority grouping khong co subpriority.
 */
#define configPRIO_BITS                         __NVIC_PRIO_BITS
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY  15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
#define configKERNEL_INTERRUPT_PRIORITY          \
    (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY     \
    (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

#define configASSERT(x)                         \
    do {                                        \
        if (!(x)) {                             \
            __disable_irq();                    \
            while (1) {                         \
            }                                   \
        }                                       \
    } while (0)

/* Ten handler khop startup_stm32f103xb.s.
 * SysTick thuoc RTOS: khong khoi tao lai bang Delay_Init().
 */
#define vPortSVCHandler                         SVC_Handler
#define xPortPendSVHandler                      PendSV_Handler
#define xPortSysTickHandler                     SysTick_Handler

#endif /* FREERTOS_CONFIG_H */
