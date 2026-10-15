#ifndef CMSIS_OS_H
#define CMSIS_OS_H

#include <stdint.h>
#include <stddef.h>

#define tskIDLE_PRIORITY            ((UBaseType_t)0U)
#define configTICK_RATE_HZ          1000

typedef void * TaskHandle_t;
typedef void (*TaskFunction_t)( void * );
typedef uint32_t TickType_t;
typedef unsigned int UBaseType_t;
typedef long BaseType_t;

#define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000U ) )

BaseType_t xTaskCreate( TaskFunction_t pxTaskCode,
                        const char * const pcName,
                        const uint16_t usStackDepth,
                        void * const pvParameters,
                        UBaseType_t uxPriority,
                        TaskHandle_t * const pxCreatedTask );

void vTaskDelay( const TickType_t xTicksToDelay );
void vTaskStartScheduler( void );

#endif /* CMSIS_OS_H */
