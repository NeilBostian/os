/* Check if the compiler thinks if we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

#include <stddef.h>
#include "../terminal.h"

typedef struct
{
    unsigned int flags;
    unsigned int mem_lower;
    unsigned int mem_upper;
    unsigned int boot_device;
    char* cmdline;

    unsigned int mods_count;
    unsigned int mods_addr;

    unsigned int syms_tabsize;
    unsigned int syms_strsize;
    unsigned int syms_addr;

    unsigned int mmap_length;
    unsigned int mmap_addr;
    unsigned int config_table;
    char* boot_loader_name;
} boot_information;

boot_information *boot_info;

void entry()
{
    terminal_clear();
    terminal_write();

    while (1)
        ;
}