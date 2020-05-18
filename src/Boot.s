.intel_syntax noprefix

.set STACK_SIZE, 0x00800000
.set MULTIBOOT_MAGIC,         0x1badb002
.set MULTIBOOT_PAGE_ALIGN,    1 << 0x00
.set MULTIBOOT_MEMORY_INFO,   1 << 0x01
.set MULTIBOOT_VIDEO_MODE,    1 << 0x02 # When included in MULTIBOOT_FLAGS, indicates the use of Multiboot Video Settings below
.set MULTIBOOT_EXPLICIT_LOAD, 1 << 0x10
.set MULTIBOOT_FLAGS,         MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
.set MULTIBOOT_CHECKSUM,      -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

.global start

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
    cli
    cld

    # Register ebx stores a pointer to a boot_information struct to access multiboot information
    # See https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Boot-information-format
    push ebx

    call entry
