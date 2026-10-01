TARGET = main

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -mcpu=cortex-m3 \
         -mthumb \
         -O0 \
         -Wall \
         -ffreestanding \
         -nostdlib

LDFLAGS = -T linker.ld \
          -nostdlib \
          -Wl,--gc-sections

all: $(TARGET).bin

$(TARGET).elf: main.o MAX7219.o startup.o
	$(CC) $(LDFLAGS) main.o MAX7219.o startup.o -o $(TARGET).elf

main.o: main.c MAX7219.h
	$(CC) $(CFLAGS) -c main.c -o main.o

MAX7219.o: MAX7219.c MAX7219.h
	$(CC) $(CFLAGS) -c MAX7219.c -o MAX7219.o

startup.o: startup.s
	$(CC) $(CFLAGS) -c startup.s -o startup.o

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin

clean:
	rm -f *.o *.elf *.bin

.PHONY: all clean
