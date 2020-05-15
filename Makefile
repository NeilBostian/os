SHELL := /usr/bin/bash

# Recursive wildcard, https://stackoverflow.com/a/18258352/3685882
rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC := ./src
OBJ := ./bin/obj
CXX_SRC_FILES := $(call rwildcard,$(SRC),*.cpp)
AS_SRC_FILES := $(call rwildcard,$(SRC),*.s)
CXX_OBJ_FILES := $(patsubst $(SRC)/%.cpp,$(OBJ)/%_cpp.o,$(CXX_SRC_FILES))
AS_OBJ_FILES := $(patsubst $(SRC)/%.s,$(OBJ)/%_s.o,$(AS_SRC_FILES))

CXX := ./toolchain/bin/i686-elf-g++
AS := ./toolchain/bin/i686-elf-as

CXXFLAGS := -ffreestanding -nostdlib -Werror -c -I$(SRC)
ASFLAGS := -c

all: run_iso

link: prep $(CXX_OBJ_FILES) $(AS_OBJ_FILES)
	$(CXX) \
		$(CXX_OBJ_FILES) \
		$(AS_OBJ_FILES) \
		-o bin/iso/boot/kernel.bin \
		-T ./src/linker.ld \
		-e start \
		-ffreestanding -nostdlib

prep:
	rm -rf ./bin
	mkdir -p ./bin/obj
	mkdir -p ./bin/iso/boot/grub

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

# Build c++ (*.cpp) files
$(OBJ)/%_cpp.o: $(SRC)/%.cpp
	@mkdir -p $$(dirname $@)
	$(CXX) $< -o $@ $(CXXFLAGS)

# Build assembly (*.s) files
$(OBJ)/%_s.o: $(SRC)/%.s
	@mkdir -p $$(dirname $@)
	$(AS) $< -o $@ $(ASFLAGS)
