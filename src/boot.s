.intel_syntax noprefix

.set MULTIBOOT_MAGIC,         0x1badb002
.set MULTIBOOT_PAGE_ALIGN,    0x1
.set MULTIBOOT_MEMORY_INFO,   0x2
.set MULTIBOOT_VIDEO_MODE,    0x4 # When included in MULTIBOOT_FLAGS, indicates the use of Multiboot Video Settings below
.set MULTIBOOT_FLAGS,         MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO # | MULTIBOOT_VIDEO_MODE
.set MULTIBOOT_CHECKSUM,      -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

.global start
.extern stack_top

# Multiboot Section
# See https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#The-layout-of-Multiboot-header
multiboot_start:
.section .multiboot
.align 4

.long MULTIBOOT_MAGIC
.long MULTIBOOT_FLAGS
.long MULTIBOOT_CHECKSUM

/* for MULTIBOOT_MEMORY_INFO */
.long 0x00000000    /* header_addr */
.long 0x00000000    /* load_addr */
.long 0x00000000    /* load_end_addr */
.long 0x00000000    /* bss_end_addr */
.long 0x00000000    /* entry_addr */

# Multiboot video settings
# See https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Header-graphics-fields
.long 0    /* mode_type */
.long 0    /* width */
.long 0    /* height */
.long 0    /* depth (always 0 for text mode) */

.section .text
start:
    # Register ebx stores a pointer to a boot_information struct to access multiboot information
    # See https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Boot-information-format
    mov boot_info, ebx
    cli
    cld

    mov ebp, stack_top
    mov esp, ebp

    jmp entry
