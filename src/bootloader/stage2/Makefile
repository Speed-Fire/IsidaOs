BUILD_DIR?=build/
ASM?=nasm
ASMFLAGS?=-f obj
CC16?=ia16-elf-gcc
CFLAGS16?=-ffreestanding -march=i8086 -mcmodel=small -fno-stack-protector -fno-pic -fno-pie -fno-builtin -fno-asynchronous-unwind-tables -fno-unwind-tables -Wall -nostdlib
LD16?=ia16-elf-ld

SOURCES_C=$(wildcard *.c)
SOURCES_ASM=$(wildcard *.asm)
OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/stage2/c/%.obj, $(SOURCES_C))
OBJECTS_ASM=$(patsubst %.asm, $(BUILD_DIR)/stage2/asm/%.obj, $(SOURCES_ASM))

.PHONY: all clean

stage2: $(BUILD_DIR)/stage2.bin
$(BUILD_DIR)/stage2.bin: $(OBJECTS_C) $(OBJECTS_ASM)
	$(LD16) -T linker.ld -Map=$(BUILD_DIR)/stage2.map -o $@ $(OBJECTS_ASM) $(OBJECTS_C)

$(BUILD_DIR)/stage2/c/%.obj: %.c always
	$(CC16) $(CFLAGS16) -c $< -o $@

$(BUILD_DIR)/stage2/asm/%.obj: %.asm always
	$(ASM) -f elf32 -o $@ $<

always:
	mkdir -p $(BUILD_DIR)/stage2/c
	mkdir -p $(BUILD_DIR)/stage2/asm

clean:
	rm -f $(BUILD_DIR)/stage2.bin
