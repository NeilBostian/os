SHELL := /usr/bin/bash

# Recursive wildcard, https://stackoverflow.com/a/18258352/3685882
rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC := ./src
OBJ := ./bin/obj
CC_SRC_FILES := $(call rwildcard,$(SRC),*.c)
AS_SRC_FILES := $(call rwildcard,$(SRC),*.s)
CC_OBJ_FILES := $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(CC_SRC_FILES))
AS_OBJ_FILES := $(patsubst $(SRC)/%.s,$(OBJ)/%_s.o,$(AS_SRC_FILES))

CC := ./toolchain/bin/i686-elf-gcc
AS := ./toolchain/bin/i686-elf-as

CFLAGS := -ffreestanding -nostdlib -Werror -c -I$(SRC)
ASFLAGS := -c

all: run_iso

prep:
	@rm -rf bin
	@mkdir bin
	@mkdir -p ./bin/obj
	@mkdir -p ./bin/iso
	@mkdir -p ./bin/iso/boot
	@mkdir -p ./bin/iso/boot/grub

link: prep $(CC_OBJ_FILES) $(AS_OBJ_FILES)
	$(CC) \
		$(call rwildcard,$(OBJ),*.o) \
		-o bin/iso/boot/kernel.bin \
		-T ./src/linker.ld \
		-e start \
		-ffreestanding -nostdlib

build_iso: link
	# Move our grub config file to the dir for building our iso
	cp ./src/grub.cfg ./bin/iso/boot/grub/grub.cfg

	# Create iso
	grub-mkrescue \
		-o ./bin/kernel.iso \
		-p /boot/grub \
		./bin/iso

run_img: link
	# Launch qemu emulator with pointed at our iso, 1GB RAM
	qemu-system-i386 \
		-m 64 \
		-kernel ./bin/iso/boot/kernel.bin

run_iso: build_iso
	# Launch qemu emulator with pointed at our iso, 1GB RAM
	qemu-system-i386 \
		-m 64 \
		-cdrom ./bin/kernel.iso

# Build c (*.c) files
$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $$(dirname $@)
	$(CC) $< -o $@ $(CFLAGS)

# Build assembly (*.s) files
$(OBJ)/%_s.o: $(SRC)/%.s
	@mkdir -p $$(dirname $@)
	$(AS) $< -o $@ $(ASFLAGS)
