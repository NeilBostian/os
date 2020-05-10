CC=./toolchain/bin/i686-elf-gcc
AS=./toolchain/bin/i686-elf-as

CFLAGS=-ffreestanding -nostdlib -Werror -c

all: prep obj iso run

prep:
	rm -rf bin
	mkdir bin
	mkdir -p ./bin/obj
	mkdir -p ./bin/iso
	mkdir -p ./bin/iso/boot
	mkdir -p ./bin/iso/boot/grub

obj:
	$(AS) ./src/boot.s -o ./bin/obj/boot_s.o
	$(CC) ./src/boot.c -o ./bin/obj/boot_c.o $(CFLAGS)
	$(CC) ./src/terminal.c -o ./bin/obj/terminal.o $(CFLAGS)

	$(CC) \
		./bin/obj/*.o \
		-o bin/iso/boot/kernel.bin \
		-T ./src/linker.ld \
		-e start \
		-ffreestanding -nostdlib

iso:
	cp ./src/grub.cfg ./bin/iso/boot/grub/grub.cfg
	grub-mkrescue \
		-o ./bin/kernel.iso \
		-p /boot/grub \
		./bin/iso

run:
	qemu-system-i386 -cdrom ./bin/kernel.iso