#pragma once

#include <Types.h>

// See https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
// Also https://docs.oracle.com/cd/E19683-01/817-3677/6mj8mbtc9/index.html
typedef enum
{
    SHT_NULL = 0x0,
    SHT_PROGBITS = 0x1,
    SHT_SYMTAB = 0x2,
    SHT_STRTAB = 0x3,
    SHT_RELA = 0x4,
    SHT_HASH = 0x5,
    SHT_DYNAMIC = 0x6,
    SHT_NOTE = 0x7,
    SHT_NOBITS = 0x8,
    SHT_REL = 0x9,
    SHT_SHLIB = 0x0A,
    SHT_DYNSYM = 0x0B,
    SHT_INIT_ARRAY = 0x0E,
    SHT_FINI_ARRAY = 0x0F,
    SHT_PREINIT_ARRAY = 0x10,
    SHT_GROUP = 0x11,
    SHT_SYMTAB_SHNDX = 0x12,
    SHT_NUM = 0x13
} elf_secttype;

typedef struct
{
    void *name_offset;
    elf_secttype sh_type;
    uint32 sh_flags;
    void *sh_addr;
    uint32 sh_offset;
    uint32 sh_size;
    uint32 sh_link;
    uint32 sh_info;
    uint32 sh_addralign;
    uint32 sh_entsize;
} __attribute__((packed)) elf32_section_header;

// See https://docs.oracle.com/cd/E23824_01/html/819-0690/chapter6-79797.html
typedef struct
{
    uint32 st_name;
    uint32 st_value;
    uint32 st_size;
    uint8 st_info;
    uint8 st_other;
    uint16 st_shndx;
} elf32_symtab_entry;
