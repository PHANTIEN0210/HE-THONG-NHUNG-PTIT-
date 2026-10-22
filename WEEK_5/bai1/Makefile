# --- Khai báo Toolchain ---
CC = arm-none-eabi-gcc
AS = arm-none-eabi-gcc -x assembler-with-cpp
OBJCOPY = arm-none-eabi-objcopy
SZ = arm-none-eabi-size

# --- Cấu hình dự án và thư mục ---
PROJECT = RTOS_Bai1
BUILD_DIR = Build
BIN_DIR = Bin
SRC_DIR = Src
LIB_DIR = Lib
LDSCRIPT = Linker/STM32F103C8Tx_FLASH.ld

# --- Cờ biên dịch ---
MCU = -mcpu=cortex-m3 -mthumb
OPT = -O2
CFLAGS = $(MCU) $(OPT) -Wall -g -fdata-sections -ffunction-sections -DSTM32F103xB

# --- Đường dẫn Include Header ---
C_INCLUDES = \
-I$(SRC_DIR) \
-I$(LIB_DIR) \
-I$(LIB_DIR)/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Inc \
-I$(LIB_DIR)/STM32CubeF1/Drivers/CMSIS/Device/ST/STM32F1xx/Include \
-I$(LIB_DIR)/STM32CubeF1/Drivers/CMSIS/Include \
-I$(LIB_DIR)/FreeRTOS-Kernel/include \
-I$(LIB_DIR)/FreeRTOS-Kernel/portable/GCC/ARM_CM3

CFLAGS += $(C_INCLUDES)

# --- Khai báo các file mã nguồn C ---
C_SOURCES = \
$(SRC_DIR)/main.c \
$(SRC_DIR)/led_button.c \
$(SRC_DIR)/system_stm32f1xx.c \
$(SRC_DIR)/stm32f10x_it.c \
$(LIB_DIR)/FreeRTOS-Kernel/list.c \
$(LIB_DIR)/FreeRTOS-Kernel/queue.c \
$(LIB_DIR)/FreeRTOS-Kernel/tasks.c \
$(LIB_DIR)/FreeRTOS-Kernel/timers.c \
$(LIB_DIR)/FreeRTOS-Kernel/portable/GCC/ARM_CM3/port.c \
$(LIB_DIR)/FreeRTOS-Kernel/portable/MemMang/heap_4.c

# --- Khai báo file mã nguồn Assembly (Startup) ---
ASM_SOURCES = \
$(SRC_DIR)/startup_stm32f103c8tx.s

# --- Cờ Linker ---
LDFLAGS = $(MCU) -T$(LDSCRIPT) -Wl,-Map=$(BUILD_DIR)/$(PROJECT).map,--cref -Wl,--gc-sections -lc -lm -lnosys

# --- Chuyển đổi file ---
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

# --- Quy tắc Build ---
all: $(BIN_DIR)/$(PROJECT).elf $(BIN_DIR)/$(PROJECT).hex $(BIN_DIR)/$(PROJECT).bin

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BIN_DIR)/$(PROJECT).elf: $(OBJECTS) Makefile | $(BIN_DIR)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BIN_DIR)/$(PROJECT).hex: $(BIN_DIR)/$(PROJECT).elf | $(BIN_DIR)
	$(OBJCOPY) -O ihex $< $@

$(BIN_DIR)/$(PROJECT).bin: $(BIN_DIR)/$(PROJECT).elf | $(BIN_DIR)
	$(OBJCOPY) -O binary -S $< $@

$(BUILD_DIR):
	mkdir -p $@

$(BIN_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	
	
# --- Lệnh nạp code xuống mạch qua ST-Link ---
flash: $(BIN_DIR)/$(PROJECT).hex
	sudo openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $< verify reset exit"
