.syntax unified
.cpu cortex-m3
.thumb

.global _estack
.global Reset_Handler

.extern main

.section .isr_vector, "a", %progbits
.word _estack
.word Reset_Handler

.section .text.Reset_Handler
.type Reset_Handler, %function

Reset_Handler:
    bl main

stop:
    b stop

.size Reset_Handler, .-Reset_Handler
