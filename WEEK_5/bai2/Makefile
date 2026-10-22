TARGET = tuan5
BUILD_DIR = Build
BIN_DIR = Bin

CC = arm-none-eabi-gcc
OPT = -O2 -g3 -Wall -Wextra -ffreestanding
MCU = -mcpu=cortex-m3 -mthumb

C_SOURCES =  \
Src/main.c \
Src/uart.c \
Src/button.c \
Src/system_stm32f1xx.c \
Lib/FreeRTOS-Kernel/tasks.c \
Lib/FreeRTOS-Kernel/queue.c \
Lib/FreeRTOS-Kernel/list.c \
Lib/FreeRTOS-Kernel/portable/GCC/ARM_CM3/port.c \
Lib/FreeRTOS-Kernel/portable/MemMang/heap_4.c

ASM_SOURCES =  \
Lib/STM32CubeF1/Drivers/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f103xb.s

C_INCLUDES =  \
-ISrc \
-ILib/STM32CubeF1/Drivers/CMSIS/Device/ST/STM32F1xx/Include \
-ILib/STM32CubeF1/Drivers/CMSIS/Include \
-ILib/FreeRTOS-Kernel/include \
-ILib/FreeRTOS-Kernel/portable/GCC/ARM_CM3

LDSCRIPT = Linker/STM32F103C8.ld

CFLAGS = $(MCU) $(C_INCLUDES) $(OPT) -DSTM32F103xB -ffunction-sections -fdata-sections -std=gnu11 -MMD -MP
LDFLAGS = $(MCU) -T$(LDSCRIPT) -nostdlib -lc -lgcc -Wl,--gc-sections -Wl,-Map=$(BIN_DIR)/$(TARGET).map

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

all: $(BIN_DIR)/$(TARGET).elf $(BIN_DIR)/$(TARGET).bin

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.s | $(BUILD_DIR)
	$(CC) -c $(MCU) -g3 $< -o $@

$(BIN_DIR)/$(TARGET).elf: $(OBJECTS) | $(BIN_DIR)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(BIN_DIR)/$(TARGET).bin: $(BIN_DIR)/$(TARGET).elf | $(BIN_DIR)
	arm-none-eabi-objcopy -O binary $< $@

$(BUILD_DIR):
	mkdir -p $@

$(BIN_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

flash: all
	openocd -f interface/stlink.cfg -c "set CPUTAPID 0" -f target/stm32f1x.cfg -c "program $(BIN_DIR)/$(TARGET).elf verify reset exit"

-include $(wildcard $(BUILD_DIR)/*.d)
