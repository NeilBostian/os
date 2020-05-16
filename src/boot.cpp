#include <boot.h>
#include "cpu.h"
#include "terminal.h"
#include "types.h"
#include "drivers/ATA.h"
#include "debug.h"

__attribute__((aligned(4096))) static uint8 stack[STACK_SIZE];
void *stack_bottom = stack;
void *stack_top = stack + STACK_SIZE;

static boot_information *boot_info;
static void print_boot_info();
static void entry_cpp();

extern "C" __attribute__((noreturn)) void entry(boot_information *lboot_info)
{
    // This has to be prior to changing the stack
    boot_info = lboot_info;

    // Setup stack
    asm("mov %0, %%ebp" ::"m"(stack_top));
    asm("mov %ebp, %esp");

    // Real code begins here
    terminal_clear();
    create_gdt();
    create_idt();

    print_boot_info();

    // terminal_writeline("");
    // terminal_writeline("Testing ATA");
    // test_atapio();
    // terminal_writeline("Test complete.");
    // terminal_pagetop();

    asm("int $0x30");

    while (1)
    {
        asm("hlt");
    }
}

void print_boot_info()
{
    terminal_writeline("boot_info");
    terminal_write(" flags ");
    terminal_write_uint8bin(boot_info->flags3);
    terminal_write_uint8bin(boot_info->flags2);
    terminal_write_uint8bin(boot_info->flags1);
    terminal_write_uint8bin(boot_info->flags0);
    terminal_write("\n");
    terminal_writeline("       |      |       |       |       |");
    terminal_writeline("       31     24      16      8       0");
    terminal_write(" stack_top:   ");
    terminal_write_uint32((uint32)stack_top);
    terminal_write("\n");
    terminal_write(" stack_bot:   ");
    terminal_write_uint32((uint32)stack_bottom);
    terminal_write("\n\n");

    if (boot_info->flags0 & BOOT_MEM)
    {
        terminal_write(" mem_lower:   ");
        terminal_write_uint32(1024 * boot_info->mem_lower);
        terminal_write("\n");

        terminal_write(" mem_upper:   ");
        terminal_write_uint32(1024 * boot_info->mem_upper);
        terminal_write("\n");
    }

    if (boot_info->flags0 & BOOT_DEVICE)
    {
        terminal_write(" boot_device: ");
        terminal_write_uint32(boot_info->boot_device);
        terminal_write("\n");
    }

    if (boot_info->flags0 & BOOT_CMDLINE)
    {
        terminal_write(" cmd: \"");
        terminal_write(boot_info->cmdline);
        terminal_write("\"\n");
    }

    if (boot_info->flags1 & BOOT_BOOTLOADER_NAME)
    {
        terminal_write(" bootloader: \"");
        terminal_write(boot_info->boot_loader_name);
        terminal_write("\"\n");
    }

    if (boot_info->flags0 & BOOT_MMAP)
    {
        terminal_writeline("");
        terminal_writeline("Detected mmap struct");

        terminal_write(" mmap_addr:   ");
        terminal_write_uint32((uint32)boot_info->mmap_addr);
        terminal_write("\n");

        terminal_write(" mmap_length: ");
        terminal_write_uint32(boot_info->mmap_length);
        terminal_write("\n");

        uint16 ct = 0;
        uint32 max_addr = (uint32)boot_info->mmap_addr + boot_info->mmap_length;
        uint32 offset = (uint32)boot_info->mmap_addr;
        mmap_entry *prev = 0;
        for (; offset < max_addr; ct++)
        {
            mmap_entry *entry = (mmap_entry *)offset;

            terminal_write("  ");
            terminal_write_uint8(ct);
            terminal_write(" sz=");
            terminal_write_uint8(entry->size);
            terminal_write(" base=");
            if ((uint32)entry->base_addr_upper != 0)
            {
                terminal_write_uint32(entry->base_addr_upper);
            }
            terminal_write_uint32(entry->base_addr_lower);
            terminal_write(" len=");
            terminal_write_uint32(entry->length);
            terminal_write(" type=");
            terminal_write_uint8(entry->type);
            terminal_write(" end=");
            terminal_write_uint32(entry->base_addr_lower + entry->length - 1);

            if (prev != 0)
            {
                uint32 prev_end = prev->base_addr_lower + prev->length;
                if (prev_end != entry->base_addr_lower)
                {
                    terminal_write(" gap=");
                    terminal_write_uint32(entry->base_addr_lower - prev_end);
                }
            }

            terminal_write("\n");

            offset += entry->size + 4;

            prev = entry;
        }
    }

    if (boot_info->flags0 & BOOT_SYMBOL_TABLE)
    {
        terminal_writeline("");
        terminal_writeline("Detected symbol table:");

        terminal_write("Address:  0x");
        terminal_write_uint32(boot_info->u.sym_tab.addr);
        terminal_writeline("");

        terminal_write("str_size: 0x");
        terminal_write_uint32(boot_info->u.sym_tab.str_size);
        terminal_writeline("");

        terminal_write("tab_size: 0x");
        terminal_write_uint32(boot_info->u.sym_tab.tab_size);
        terminal_writeline("");

        dbg_print_memory((void *)boot_info->u.sym_tab.addr, 256);
    }

    if (boot_info->flags0 & BOOT_ELF_SECTION_HEADER)
    {
        terminal_writeline("");
        terminal_writeline("Detected ELF header table:");

        terminal_write("Address:  0x");
        terminal_write_uint32((uint32)boot_info->u.elf_info.header);
        terminal_writeline("");

        terminal_write("Num:      0x");
        terminal_write_uint32(boot_info->u.elf_info.entry_count);
        terminal_writeline("");

        terminal_write("Size:     0x");
        terminal_write_uint32(boot_info->u.elf_info.entry_size);
        terminal_writeline("");

        terminal_write("Sections: 0x");
        terminal_write_uint32(boot_info->u.elf_info.section_header_index);
        terminal_writeline("");

        elf32_section_header shstrtab_sec = boot_info->u.elf_info.header[boot_info->u.elf_info.section_header_index];
        void *shstrtab = shstrtab_sec.sh_addr;
        terminal_write("Found strtab at ");
        terminal_write_uint32((uint32)shstrtab);
        terminal_writeline("");

        elf32_section_header *ptr = boot_info->u.elf_info.header;
        for (int i = 0; i < boot_info->u.elf_info.entry_count; i++)
        {
            if (ptr->sh_type != SHT_NULL)
            {
                char *name = (char *)((uint32)shstrtab + (uint32)ptr->name_offset);
                terminal_write("ind=");
                terminal_write_uint8((uint8)i);
                terminal_write(", type=");
                terminal_write_uint8(ptr->sh_type);
                terminal_write(", addr=");
                terminal_write_uint32((uint32)ptr->sh_addr);
                terminal_write(", name=\"");
                terminal_write(name);
                terminal_write(", e_sz=");
                terminal_write_uint8(ptr->sh_entsize);
                terminal_write(", sh_sz=");
                terminal_write_uint32(ptr->sh_size);
                terminal_write("\"\n");
            }

            ptr++;
        }

        terminal_writeline("");

        elf32_section_header shsymtab = boot_info->u.elf_info.header[7];
        int numEntries = shsymtab.sh_size / shsymtab.sh_entsize;
        elf32_symtab_entry *symtab = (elf32_symtab_entry *)shsymtab.sh_addr;
        terminal_write("Found symtab at ");
        terminal_write_uint32((uint32)symtab);
        terminal_writeline("");

        void* strtab = boot_info->u.elf_info.header[8].sh_addr;

        for (int i = 0; i < numEntries; i++)
        {
            elf32_symtab_entry e = symtab[i];
            elf32_section_header h = boot_info->u.elf_info.header[e.st_shndx];
            string sym_name = (string)((uint32)strtab + e.st_name);
            string sect_name = (string)((uint32)shstrtab + (uint32)h.name_offset);

            terminal_write("Addr 0x");
            terminal_write_uint32(e.st_value);
            terminal_write(", name=");
            //terminal_write_uint32(e.st_name);
            if (e.st_name != 0)
            {
                terminal_write(sym_name);
            }
            terminal_write(", section=");
            terminal_write(sect_name);
            terminal_write(", type=");

            uint8 type = e.st_info & 0x0F;

            switch (type)
            {
            case 0x00:
                terminal_write("None");
                break;
            case 0x01:
                terminal_write("Obj");
                break;
            case 0x02:
                terminal_write("Func");
                break;
            case 0x03:
                terminal_write("Sect");
                break;
            case 0x04:
                terminal_write("File");
                break;
            }

            terminal_writeline("");
        }
    }

    if (boot_info->flags0 & BOOT_DRIVES)
    {
        terminal_writeline("Dectected drives");
        terminal_write(" drives_addr: ");
        terminal_write_uint32((uint32)boot_info->drives_addr);
        terminal_write("\n");

        terminal_write(" drives_len:  ");
        terminal_write_uint32(boot_info->drives_length);
        terminal_write("\n");
    }
}
