#include "cpu.h"
#include "terminal.h"
#include "types.h"
#include "drivers/ATA_PIO_LBA28.h"

// Flags for boot_info.flags0
#define BOOT_MEM 1 << 0
#define BOOT_DEVICE 1 << 1
#define BOOT_CMDLINE 1 << 2
#define BOOT_MMAP 1 << 6

// Flags for boot_info.flags1 (beginning offset 8 in 32-bit flags)
#define BOOT_BOOTLOADER_NAME 1 << 1 // Flag 9 in 32-bit flags

typedef struct
{
    uint32 size;
    uint32 base_addr_lower;
    uint32 base_addr_upper;
    uint64 length;
    uint32 type;
} __attribute__((packed)) mmap_entry;

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
    uint32 syms_tabsize;
    uint32 syms_strsize;
    uint32 syms_addr;
    uint32 syms_reserved;

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

// .extern in boot.s, see https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Boot-information-format
boot_information *boot_info;

// .globals from boot.s, for debugging memory addresses
extern void *start;
extern void *stack_bottom;
extern void *stack_top;

void entry()
{
    terminal_clear();
    create_gdt();
    create_idt();

    char uint32_str[9];

    terminal_writeline("symbols");
    terminal_write(" start:       ");
    terminal_write_uint32((uint32)start);
    terminal_write("\n");
    terminal_write(" entry():     ");
    terminal_write_uint32((uint32)entry);
    terminal_write("\n");
    terminal_write(" stack_bot:   ");
    terminal_write_uint32((uint32)stack_bottom);
    terminal_write("\n");
    terminal_write(" stack_top:   ");
    terminal_write_uint32((uint32)stack_top);
    terminal_write("\n\n");

    terminal_writeline("boot_info");
    terminal_write(" flags ");
    terminal_write_uint8bin(boot_info->flags3);
    terminal_write_uint8bin(boot_info->flags2);
    terminal_write_uint8bin(boot_info->flags1);
    terminal_write_uint8bin(boot_info->flags0);
    terminal_write("\n");
    terminal_writeline("       |      |       |       |       |");
    terminal_writeline("       31     24      16      8       0");

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
        void *offset = boot_info->mmap_addr;
        mmap_entry *prev = 0;
        for (; (uint32)offset < max_addr; ct++)
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

    if (boot_info->flags0 & 0b10000000)
    {
        terminal_writeline("Dectected drives");
        terminal_write(" drives_addr: ");
        terminal_write_uint32((uint32)boot_info->drives_addr);
        terminal_write("\n");

        terminal_write(" drives_len:  ");
        terminal_write_uint32(boot_info->drives_length);
        terminal_write("\n");
    }

    while (1)
        ;
}