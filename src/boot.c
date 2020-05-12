#include "cpu.h"
#include "terminal.h"
#include "types.h"

typedef struct
{
    uint32 flags;
    uint32 mem_lower;
    uint32 mem_upper;
    uint32 boot_device;
    char *cmdline;

    uint32 mods_count;
    uint32 mods_addr;

    uint32 syms_tabsize;
    uint32 syms_strsize;
    uint32 syms_addr;

    uint32 mmap_length;
    uint32 mmap_addr;
    uint32 config_table;
    char *boot_loader_name;
} boot_information;

boot_information *boot_info;

void entry()
{
    terminal_clear();
    create_gdt();
    create_idt();

    char uint32_str[9];

    terminal_writeline(" -----------------");
    terminal_writeline(" | Kernel loaded |");
    terminal_writeline(" -----------------");

    terminal_write(" boot flags:  ");
    terminal_write_uint32(boot_info->flags);
    terminal_write("\n");

    terminal_write(" boot_device: ");
    terminal_write_uint32(boot_info->boot_device);
    terminal_write("\n");

    while (1)
        ;
}