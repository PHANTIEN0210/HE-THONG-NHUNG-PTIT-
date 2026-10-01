TARGET = Bai01_LCD

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -mcpu=cortex-m3 \
         -mthumb \
         -O0 \
         -Wall \
         -ffreestanding \
         -fno-builtin \
         -nostdlib

LDFLAGS = -T linker.ld \
          -nostdlib \
          -Wl,--build-id=none

all: $(TARGET).bin

$(TARGET).elf: startup.o main.o
	$(CC) $(LDFLAGS) startup.o main.o -o $(TARGET).elf

startup.o: startup.s
	$(CC) $(CFLAGS) -c startup.s -o startup.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin

clean:
	rm -f *.o *.elf *.bin

.PHONY: all clean
