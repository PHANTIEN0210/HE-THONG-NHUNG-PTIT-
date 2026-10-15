.syntax unified
.cpu cortex-m3
.thumb

.global g_pfnVectors
.global Reset_Handler

.section .text.Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
    /* Sao chép dữ liệu .data sang RAM nếu cần, ở đây dùng -nostdlib nên ta gọi trực tiếp main */
    LDR r0, =main
    BLX r0

_infinite_loop:
    B _infinite_loop

.section .isr_vector,"a",%progbits
g_pfnVectors:
    .word 0x20005000     /* Top of Stack (RAM 20KB) */
    .word Reset_Handler  /* Reset Handler */
