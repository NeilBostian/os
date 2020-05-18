#include <Cpu/Stack.h>
#include <Debug.h>
#include <Drivers/Elf.h>
#include <Drivers/MultibootInfo.h>
#include <Terminal.h>
#include <Types.h>

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

boot_information *MultibootInfo::BootInfo;

void MultibootInfo::Initialize(boot_information *lbootInfo)
{
    MultibootInfo::BootInfo = lbootInfo;
}

void MultibootInfo::PrintHeader()
{
    boot_information *boot_info = MultibootInfo::BootInfo;

    Terminal::WriteLine("boot_info");
    Terminal::Write(" flags ");
    Terminal::WriteBin(boot_info->flags3);
    Terminal::WriteBin(boot_info->flags2);
    Terminal::WriteBin(boot_info->flags1);
    Terminal::WriteBin(boot_info->flags0);
    Terminal::WriteLine();
    Terminal::WriteLine("       |      |       |       |       |");
    Terminal::WriteLine("       31     24      16      8       0");
    Terminal::Write(" stack_top:   ");
    Terminal::Write((uint32)Stack::GetTopAddress());
    Terminal::WriteLine();
    Terminal::Write(" stack_bot:   ");
    Terminal::Write((uint32)Stack::GetBottomAddress());

    if (boot_info->flags0 & BOOT_MEM)
    {
        Terminal::Write(" mem_lower:   ");
        Terminal::Write(1024 * boot_info->mem_lower);
        Terminal::WriteLine();

        Terminal::Write(" mem_upper:   ");
        Terminal::Write(1024 * boot_info->mem_upper);
        Terminal::WriteLine();
    }

    if (boot_info->flags0 & BOOT_DEVICE)
    {
        Terminal::Write(" boot_device: ");
        Terminal::Write(boot_info->boot_device);
        Terminal::WriteLine();
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
}

void MultibootInfo::PrintMmap()
{
    boot_information *boot_info = MultibootInfo::BootInfo;

    if (boot_info->flags0 & BOOT_MMAP)
    {
        Terminal::WriteLine("MMAP -> Detected mmap struct");

        Terminal::Write(" mmap_addr:   ");
        Terminal::Write((uint32)boot_info->mmap_addr);
        Terminal::WriteLine();

        Terminal::Write(" mmap_length: ");
        Terminal::Write(boot_info->mmap_length);
        Terminal::WriteLine();

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

            Terminal::WriteLine();

            offset += entry->size + 4;

            prev = entry;
        }
    }
    else
    {
        Terminal::WriteLine("MMAP -> No mmap found");
    }
}

void MultibootInfo::PrintBootSectionHeader()
{
    boot_information *boot_info = MultibootInfo::BootInfo;

    if (boot_info->flags0 & BOOT_SYMBOL_TABLE)
    {
        Terminal::WriteLine("BOOT_SYM -> Detected symbol table:");

        Terminal::Write("Address:  0x");
        Terminal::Write(boot_info->u.sym_tab.addr);
        Terminal::WriteLine();

        Terminal::Write("str_size: 0x");
        Terminal::Write(boot_info->u.sym_tab.str_size);
        Terminal::WriteLine();

        Terminal::Write("tab_size: 0x");
        Terminal::Write(boot_info->u.sym_tab.tab_size);
        Terminal::WriteLine();

        Debug::PrintMemory((void *)boot_info->u.sym_tab.addr, 256);
    }
    else
    {
        Terminal::WriteLine("BOOT_SYM -> No symbol table found.");
    }
}

void MultibootInfo::PrintElfSectionHeader()
{
    boot_information *boot_info = MultibootInfo::BootInfo;

    Terminal::WriteLine("Detected ELF header table:");

    Terminal::Write("Address:  0x");
    Terminal::Write((uint32)boot_info->u.elf_info.header);
    Terminal::WriteLine();

    Terminal::Write("Num:      0x");
    Terminal::Write(boot_info->u.elf_info.entry_count);
    Terminal::WriteLine();

    Terminal::Write("Size:     0x");
    Terminal::Write(boot_info->u.elf_info.entry_size);
    Terminal::WriteLine();

    Terminal::Write("Sections: 0x");
    Terminal::Write(boot_info->u.elf_info.section_header_index);
    Terminal::WriteLine();

    elf32_section_header shstrtab_sec = boot_info->u.elf_info.header[boot_info->u.elf_info.section_header_index];
    void *shstrtab = shstrtab_sec.sh_addr;
    Terminal::Write("Found strtab at ");
    Terminal::Write((uint32)shstrtab);
    Terminal::WriteLine();

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
}

void MultibootInfo::PrintElfSymbols()
{
    boot_information *boot_info = MultibootInfo::BootInfo;

    elf32_section_header shstrtab_sec = boot_info->u.elf_info.header[boot_info->u.elf_info.section_header_index];
    void *shstrtab = shstrtab_sec.sh_addr;

    elf32_section_header shsymtab = boot_info->u.elf_info.header[7];
    int numEntries = shsymtab.sh_size / shsymtab.sh_entsize;
    elf32_symtab_entry *symtab = (elf32_symtab_entry *)shsymtab.sh_addr;

    Terminal::Write("Found symtab at ");
    Terminal::Write((uint32)symtab);
    Terminal::WriteLine();

    void *strtab = boot_info->u.elf_info.header[8].sh_addr;

    for (int i = 0; i < numEntries; i++)
    {
        elf32_symtab_entry e = symtab[i];
        elf32_section_header h = boot_info->u.elf_info.header[e.st_shndx];
        string sym_name = (string)((uint32)strtab + e.st_name);
        string sect_name = (string)((uint32)shstrtab + (uint32)h.name_offset);

        Terminal::Write("Addr 0x");
        Terminal::Write(e.st_value);
        Terminal::Write(", name=");
        //terminal_write_uint32(e.st_name);
        if (e.st_name != 0)
        {
            Terminal::Write(sym_name);
        }
        Terminal::Write(", section=");
        Terminal::Write(sect_name);
        Terminal::Write(", type=");

        uint8 type = e.st_info & 0x0F;

        switch (type)
        {
        case 0x00:
            Terminal::Write("None");
            break;
        case 0x01:
            Terminal::Write("Obj");
            break;
        case 0x02:
            Terminal::Write("Func");
            break;
        case 0x03:
            Terminal::Write("Sect");
            break;
        case 0x04:
            Terminal::Write("File");
            break;
        }

        Terminal::WriteLine();
    }
}
