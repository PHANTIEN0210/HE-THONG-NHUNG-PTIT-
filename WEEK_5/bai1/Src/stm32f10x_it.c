#include "stm32f10x_it.h"

void NMI_Handler(void) {}

void HardFault_Handler(void) {
  /* Bị kẹt ở đây nếu xảy ra lỗi phần cứng nghiêm trọng hoặc tràn Stack */
  while (1) {}
}

void MemManage_Handler(void) {
  while (1) {}
}

void BusFault_Handler(void) {
  while (1) {}
}

void UsageFault_Handler(void) {
  while (1) {}
}

void DebugMon_Handler(void) {}
