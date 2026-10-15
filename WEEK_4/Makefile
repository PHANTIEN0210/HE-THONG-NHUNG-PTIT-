TARGET = tuan4

PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size

C_SOURCES = \
Src/main.c \
Src/led_task.c \
Src/startup.s

C_INCLUDES = \
-IInc

CPU = -mcpu=cortex-m3
MCU = $(CPU) -mthumb -mfloat-abi=soft

# Thêm -nostdlib để loại bỏ hoàn toàn các lời gọi thư viện chuẩn gây lỗi kích thước
CFLAGS = $(MCU) $(C_INCLUDES) -O0 -Wall -fdata-sections -ffunction-sections -nostdlib -T linker.ld

all: $(TARGET).elf $(TARGET).bin

$(TARGET).elf: $(C_SOURCES)
	$(CC) $(CFLAGS) $(C_SOURCES) -o $@
	$(SZ) $@

%.bin: %.elf
	$(CP) -O binary $< $@

clean:
	rm -f *.elf *.bin
