#pragma once

#include "elf.h"
#include "types.h"

#define STACK_SIZE 1024 * 64

// Flags for boot_info.flags0
#define BOOT_MEM 1 << 0
#define BOOT_DEVICE 1 << 1
#define BOOT_CMDLINE 1 << 2
#define BOOT_SYMBOL_TABLE 1 << 4
#define BOOT_ELF_SECTION_HEADER 1 << 5
#define BOOT_MMAP 1 << 6
#define BOOT_DRIVES 1 << 7

// Flags for boot_info.flags1 (beginning offset 8 in 32-bit flags)
#define BOOT_BOOTLOADER_NAME 1 << 1 // Flag 9 in 32-bit flags

typedef struct
{
    uint32 tab_size;
    uint32 str_size;
    uint32 addr;
    uint32 __reserved;
} symbol_table;

typedef struct
{
    uint32 entry_count;
    uint32 entry_size;
    elf32_section_header *header;
    uint32 section_header_index;
} __attribute__((packed)) boot_elf_info;

typedef struct
{
    uint32 size;
    uint32 base_addr_lower;
    uint32 base_addr_upper;
    uint64 length;
    uint32 type;
} __attribute__((packed)) mmap_entry;

// See https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Boot-information-format
typedef struct
{
    /* 0-3 */
    uint8 flags0;
    uint8 flags1;
    uint8 flags2;
    uint8 flags3;

    /* 4-11 */
    uint32 mem_lower;
    uint32 mem_upper;

    /* 12-19 */
    uint32 boot_device;
    char *cmdline;

    /* 20-27 */
    uint32 mods_count;
    uint32 mods_addr;

    /* 28-43 */
    union {
        symbol_table sym_tab;
        boot_elf_info elf_info;
    } u;

    /* 44-51 */
    uint32 mmap_length;
    mmap_entry *mmap_addr;

    /* 52-59 */
    uint32 drives_length;
    uint32 drives_addr;

    /* 60-67 */
    uint32 config_table;
    char *boot_loader_name;
} __attribute__((packed)) boot_information;
