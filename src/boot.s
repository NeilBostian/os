.set MULTIBOOT_MAGIC,         0x1badb002

.set MULTIBOOT_PAGE_ALIGN,    0x1
.set MULTIBOOT_MEMORY_INFO,   0x2
.set MULTIBOOT_VIDEO_MODE,    0x4
.set multiboot_flags,         MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO # | MULTIBOOT_VIDEO_MODE

.set multiboot_checksum,      -(MULTIBOOT_MAGIC + multiboot_flags)

# Multiboot Section
# See https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#The-layout-of-Multiboot-header
.section .multiboot
.align 4

.long MULTIBOOT_MAGIC
.long multiboot_flags
.long multiboot_checksum

/* for MULTIBOOT_MEMORY_INFO */
.long 0x00000000    /* header_addr */
.long 0x00000000    /* load_addr */
.long 0x00000000    /* load_end_addr */
.long 0x00000000    /* bss_end_addr */
.long 0x00000000    /* entry_addr */

# Multiboot video settings
# See https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Header-graphics-fields
.extern VGA_WIDTH
.extern VGA_HEIGHT

.long 0x00000001    /* mode_type */
.long 120     /* width */
.long 120    /* height */
.long 0             /* depth (always 0 for text mode) */

/* Init 16KB stack */
.section .stack, "aw", @nobits
stack_bottom:
.skip 16384
stack_top:

.section .text
.global start
.extern boot_info

start:
    # Register ebx stores a pointer to a boot_information struct to access multiboot information
    # See https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Boot-information-format
    mov %ebx, boot_info
    cli
    cld

    call entry

loop:
    hlt
    jmp loop
