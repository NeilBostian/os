#include "../gdt.h"
#include "../terminal.h"
#include "../types.h"

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
    create_gdt();
    terminal_clear();

    while (1)
        ;
}