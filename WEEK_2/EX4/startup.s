.syntax unified
.cpu cortex-m3
.thumb

.global vtable
.global Reset_Handler

.section .vectors
vtable:
    .word 0x20005000
    .word Reset_Handler

.section .text
.thumb_func
Reset_Handler:
    bl main
    b .
