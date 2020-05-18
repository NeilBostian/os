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
    Terminal::Clear();
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
    Terminal::WriteLine("boot_info");
    Terminal::Write(" flags ");
    Terminal::WriteBin(boot_info->flags3);
    Terminal::WriteBin(boot_info->flags2);
    Terminal::WriteBin(boot_info->flags1);
    Terminal::WriteBin(boot_info->flags0);
    Terminal::Write("\n");
    Terminal::WriteLine("       |      |       |       |       |");
    Terminal::WriteLine("       31     24      16      8       0");
    Terminal::Write(" stack_top:   ");
    Terminal::Write((uint32)stack_top);
    Terminal::Write("\n");
    Terminal::Write(" stack_bot:   ");
    Terminal::Write((uint32)stack_bottom);
    Terminal::Write("\n\n");

    if (boot_info->flags0 & BOOT_MEM)
    {
        Terminal::Write(" mem_lower:   ");
        Terminal::Write(1024 * boot_info->mem_lower);
        Terminal::Write("\n");

        Terminal::Write(" mem_upper:   ");
        Terminal::Write(1024 * boot_info->mem_upper);
        Terminal::Write("\n");
    }

    if (boot_info->flags0 & BOOT_DEVICE)
    {
        Terminal::Write(" boot_device: ");
        Terminal::Write(boot_info->boot_device);
        Terminal::Write("\n");
    }

    if (boot_info->flags0 & BOOT_CMDLINE)
    {
        Terminal::Write(" cmd: \"");
        Terminal::Write(boot_info->cmdline);
        Terminal::Write("\"\n");
    }

    if (boot_info->flags1 & BOOT_BOOTLOADER_NAME)
    {
        Terminal::Write(" bootloader: \"");
        Terminal::Write(boot_info->boot_loader_name);
        Terminal::Write("\"\n");
    }

    if (boot_info->flags0 & BOOT_MMAP)
    {
        Terminal::WriteLine("");
        Terminal::WriteLine("Detected mmap struct");

        Terminal::Write(" mmap_addr:   ");
        Terminal::Write((uint32)boot_info->mmap_addr);
        Terminal::Write("\n");

        Terminal::Write(" mmap_length: ");
        Terminal::Write(boot_info->mmap_length);
        Terminal::Write("\n");

        uint16 ct = 0;
        uint32 max_addr = (uint32)boot_info->mmap_addr + boot_info->mmap_length;
        uint32 offset = (uint32)boot_info->mmap_addr;
        mmap_entry *prev = 0;
        for (; offset < max_addr; ct++)
        {
            mmap_entry *entry = (mmap_entry *)offset;

            Terminal::Write("  ");
            Terminal::Write((uint8)ct);
            Terminal::Write(" sz=");
            Terminal::Write((uint8)entry->size);
            Terminal::Write(" base=");
            if ((uint32)entry->base_addr_upper != 0)
            {
                Terminal::Write(entry->base_addr_upper);
            }
            Terminal::Write(entry->base_addr_lower);
            Terminal::Write(" len=");
            Terminal::Write((uint32)entry->length);
            Terminal::Write(" type=");
            Terminal::Write((uint8)entry->type);
            Terminal::Write(" end=");
            Terminal::Write((uint32)(entry->base_addr_lower + entry->length - 1));

            if (prev != 0)
            {
                uint32 prev_end = prev->base_addr_lower + prev->length;
                if (prev_end != entry->base_addr_lower)
                {
                    Terminal::Write(" gap=");
                    Terminal::Write(entry->base_addr_lower - prev_end);
                }
            }

            Terminal::Write("\n");

            offset += entry->size + 4;

            prev = entry;
        }
    }

    if (boot_info->flags0 & BOOT_SYMBOL_TABLE)
    {
        Terminal::WriteLine("");
        Terminal::WriteLine("Detected symbol table:");

        Terminal::Write("Address:  0x");
        Terminal::Write(boot_info->u.sym_tab.addr);
        Terminal::WriteLine("");

        Terminal::Write("str_size: 0x");
        Terminal::Write(boot_info->u.sym_tab.str_size);
        Terminal::WriteLine("");

        Terminal::Write("tab_size: 0x");
        Terminal::Write(boot_info->u.sym_tab.tab_size);
        Terminal::WriteLine("");

        dbg_print_memory((void *)boot_info->u.sym_tab.addr, 256);
    }

    if (boot_info->flags0 & BOOT_ELF_SECTION_HEADER)
    {
        Terminal::WriteLine("");
        Terminal::WriteLine("Detected ELF header table:");

        Terminal::Write("Address:  0x");
        Terminal::Write((uint32)boot_info->u.elf_info.header);
        Terminal::WriteLine("");

        Terminal::Write("Num:      0x");
        Terminal::Write(boot_info->u.elf_info.entry_count);
        Terminal::WriteLine("");

        Terminal::Write("Size:     0x");
        Terminal::Write(boot_info->u.elf_info.entry_size);
        Terminal::WriteLine("");

        Terminal::Write("Sections: 0x");
        Terminal::Write(boot_info->u.elf_info.section_header_index);
        Terminal::WriteLine("");

        elf32_section_header shstrtab_sec = boot_info->u.elf_info.header[boot_info->u.elf_info.section_header_index];
        void *shstrtab = shstrtab_sec.sh_addr;
        Terminal::Write("Found strtab at ");
        Terminal::Write((uint32)shstrtab);
        Terminal::WriteLine("");

        elf32_section_header *ptr = boot_info->u.elf_info.header;
        for (int i = 0; i < boot_info->u.elf_info.entry_count; i++)
        {
            if (ptr->sh_type != SHT_NULL)
            {
                char *name = (char *)((uint32)shstrtab + (uint32)ptr->name_offset);
                Terminal::Write("ind=");
                Terminal::Write((uint8)i);
                Terminal::Write(", type=");
                Terminal::Write((uint8)ptr->sh_type);
                Terminal::Write(", addr=");
                Terminal::Write((uint32)ptr->sh_addr);
                Terminal::Write(", name=\"");
                Terminal::Write(name);
                Terminal::Write(", e_sz=");
                Terminal::Write(ptr->sh_entsize);
                Terminal::Write(", sh_sz=");
                Terminal::Write(ptr->sh_size);
                Terminal::Write("\"\n");
            }

            ptr++;
        }

        Terminal::WriteLine("");

        // elf32_section_header shsymtab = boot_info->u.elf_info.header[7];
        // int numEntries = shsymtab.sh_size / shsymtab.sh_entsize;
        // elf32_symtab_entry *symtab = (elf32_symtab_entry *)shsymtab.sh_addr;
        // Terminal::Write("Found symtab at ");
        // Terminal::Write((uint32)symtab);
        // Terminal::WriteLine("");

        // void* strtab = boot_info->u.elf_info.header[8].sh_addr;

        // for (int i = 0; i < numEntries; i++)
        // {
        //     elf32_symtab_entry e = symtab[i];
        //     elf32_section_header h = boot_info->u.elf_info.header[e.st_shndx];
        //     string sym_name = (string)((uint32)strtab + e.st_name);
        //     string sect_name = (string)((uint32)shstrtab + (uint32)h.name_offset);

        //     Terminal::Write("Addr 0x");
        //     Terminal::Write(e.st_value);
        //     Terminal::Write(", name=");
        //     //terminal_write_uint32(e.st_name);
        //     if (e.st_name != 0)
        //     {
        //         Terminal::Write(sym_name);
        //     }
        //     Terminal::Write(", section=");
        //     Terminal::Write(sect_name);
        //     Terminal::Write(", type=");

        //     uint8 type = e.st_info & 0x0F;

        //     switch (type)
        //     {
        //     case 0x00:
        //         Terminal::Write("None");
        //         break;
        //     case 0x01:
        //         Terminal::Write("Obj");
        //         break;
        //     case 0x02:
        //         Terminal::Write("Func");
        //         break;
        //     case 0x03:
        //         Terminal::Write("Sect");
        //         break;
        //     case 0x04:
        //         Terminal::Write("File");
        //         break;
        //     }

        //     Terminal::WriteLine("");
        // }
    }

    if (boot_info->flags0 & BOOT_DRIVES)
    {
        Terminal::WriteLine("Dectected drives");
        Terminal::Write(" drives_addr: ");
        Terminal::Write((uint32)boot_info->drives_addr);
        Terminal::Write("\n");

        Terminal::Write(" drives_len:  ");
        Terminal::Write(boot_info->drives_length);
        Terminal::Write("\n");
    }
}
