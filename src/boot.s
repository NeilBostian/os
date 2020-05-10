.set MULTIBOOT_MAGIC,         0x1badb002
.set MULTIBOOT_PAGE_ALIGN,    0x1
.set MULTIBOOT_MEMORY_INFO,   0x2
.set multiboot_flags,         MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
.set multiboot_checksum,      -(MULTIBOOT_MAGIC + multiboot_flags)

.section .multiboot
.align 4

.long MULTIBOOT_MAGIC
.long multiboot_flags
.long multiboot_checksum

/* for MULTIBOOT_MEMORY_INFO */
# .long 0x00000000    /* header_addr */
# .long 0x00000000    /* load_addr */
# .long 0x00000000    /* load_end_addr */
# .long 0x00000000    /* bss_end_addr */
# .long 0x00000000    /* entry_addr */

/* for MULTIBOOT_VIDEO_MODE */
# .long 0x00000000    /* mode_type */
# .long 1280          /* width */
# .long 1024          /* height */
# .long 32            /* depth */

.section .stack, "aw", @nobits
stack_bottom:
.skip 16384
stack_top:

.section .text
.global start
.type start, @function

start:
    cli
    cld

    call entry

loop:
    hlt
    jmp loop

.section .data
