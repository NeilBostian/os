#include <Cpu/Stack.h>
#include <Debug.h>
#include <Drivers/Elf.h>
#include <Drivers/MultibootInfo.h>
#include <Terminal.h>

// Returns a pointer to the next address that will be executed after this function is called.
// AKA - returns the return address of this function
static uint32 debug_getreg_eip()
{
    return (uint32)__builtin_return_address(0);
}

// Returns register %ebp
extern "C" uint32 debug_getreg_ebp();

static string readable_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`~-_=+!@#$%^&*()[{]};:'\",<.>/?\\|";

elf32_symtab_entry *Debug::GetSymbolFromAddress(uint32 addr)
{
    elf32_section_header *symtab = MultibootInfo::GetElfSymbolTable();

    uint32 symtab_len = symtab->sh_size / symtab->sh_entsize;

    uint32 indexForAddr = 0;

    for (uint32 i = 0; i < symtab_len; i++)
    {
        elf32_symtab_entry *e = (elf32_symtab_entry *)symtab->sh_addr + i;

        uint8 type = e->st_info & 0x0F;
        if (type == 0x02 && e->st_value < addr && e->st_size > 0)
        {
            elf32_symtab_entry *currMax = (elf32_symtab_entry *)symtab->sh_addr + indexForAddr;

            if (i == 0 || e->st_value > currMax->st_value)
            {
                indexForAddr = i;
            }
        }
    }

    return (elf32_symtab_entry *)symtab->sh_addr + indexForAddr;
}

void Debug::PrintMemory(void *ptr, uint32 num_bytes)
{
    if (num_bytes % 16 != 0)
    {
        num_bytes = ((num_bytes / 16) + 1) * 16;
    }

    uint8 *val = (uint8 *)ptr;

    Terminal::Write("Memory contents beginning at 0x");
    Terminal::Write((uint32)ptr);
    Terminal::WriteLine(":");
    Terminal::WriteLine();

    uint8 nybble = (uint8)((uint32)ptr & 0x0000000F);
    Terminal::Write("           ");

    char chr[3];
    for (int i = 0; i < 16; i++)
    {
        Terminal::Write(" ");
        String::ConvertToHex(nybble, chr);
        chr[0] = 'x';
        Terminal::Write(chr);

        nybble = (nybble + 1) % 16;
    }
    Terminal::WriteLine();
    Terminal::WriteLine("           ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿");

    uint32 print_chars_len = String::Len(readable_chars);
    for (int i = 0; i < num_bytes; i++)
    {
        if (i % 16 == 0)
        {
            if (i != 0)
            {
                Terminal::Write("³ ");

                for (int j = i - 16; j < i; j++)
                {
                    char *base = (char *)ptr;
                    char c = base[j];

                    bool did_print = false;
                    for (int k = 0; k < print_chars_len; k++)
                    {
                        if (readable_chars[k] == c)
                        {
                            Terminal::Write(c, VGA_FG_LIGHT_GREY);
                            did_print = true;
                            break;
                        }
                    }

                    if (!did_print)
                    {
                        Terminal::Write('.', VGA_FG_DARK_GREY);
                    }
                }
                Terminal::WriteLine();
            }

            Terminal::Write("0x");
            Terminal::Write((uint32)val);
            Terminal::Write(" ³");
        }
        else
        {
            Terminal::Write(" ");
        }

        uint8 x = *val;
        String::ConvertToHex(x, chr);
        vga_color color = (x == 0 || x == 0xFF ? VGA_FG_DARK_GREY : VGA_FG_WHITE);

        Terminal::Write(chr[0], color);
        Terminal::Write(chr[1], color);

        val++;
    }

    Terminal::Write("³ ");

    for (int j = num_bytes - 16; j < num_bytes; j++)
    {
        char *base = (char *)ptr;
        char c = base[j];

        bool did_print = false;
        for (int k = 0; k < print_chars_len; k++)
        {
            if (readable_chars[k] == c)
            {
                Terminal::Write(c, VGA_FG_LIGHT_GREY);
                did_print = true;
                break;
            }
        }

        if (!did_print)
        {
            Terminal::Write('.', VGA_FG_DARK_GREY);
        }
    }
    Terminal::WriteLine();
    Terminal::WriteLine("           ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ");
}

void Debug::PrintStack(uint32 num_items)
{
    /*
    When this function is called, the stack looks like the below (dummy addresses)

    STACK_BOTTOM
    0xFFF4 <-- esp
    0xFFF0 <-- ebp
    0xFFEC <-- return address to caller
    0xFFE8 <-- num_items parameter
    0xFFE4 <-- last item on the stack from the caller (possibly registers or local vars)
    STACK_TOP
    */

    // EBP for this function
    uint32 ebp = debug_getreg_ebp();

    Debug::PrintStack(num_items, ebp);
}

void Debug::PrintStack(uint32 num_items, uint32 ebp)
{
    uint32 stack_top = (uint32)Stack::GetTopAddress();

    uint32 *val = (uint32 *)ebp;

    Terminal::Write("Stack contents beginning at 0x");
    Terminal::Write((uint32)val);
    Terminal::WriteLine(":");

    // Track if the previous stack item was ebp
    bool prev_ebp = false;

    for (uint32 i = 0; i < num_items && (uint32)val < stack_top; i++)
    {
        uint32 i_ptr = (uint32)val;
        uint32 i_val = *val;

        Terminal::Write("  ADDR 0x");
        Terminal::Write(i_ptr);
        Terminal::Write(": 0x");
        Terminal::Write(i_val);

        if (prev_ebp)
        {
            Terminal::Write(" <-- From ");
            Debug::WriteSymbol(i_val);
        }

        if (i_ptr == ebp)
        {
            Terminal::Write(" <-- EBP");

            ebp = i_val;
            prev_ebp = true;
        }
        else
        {
            prev_ebp = false;
        }

        val++;

        if ((uint32)val >= stack_top)
        {
            Terminal::WriteLine(" <-- Stack::Top");
        }

        Terminal::WriteLine();
    }
}

void Debug::PrintCallStack(uint32 num_items)
{
    /*
    When this function is called, the stack looks like the below (dummy addresses)

    STACK_BOTTOM
    0xFFF4 <-- esp
    0xFFF0 <-- ebp
    0xFFEC <-- return address to caller
    0xFFE8 <-- num_items parameter
    0xFFE4 <-- last item on the stack from the caller (possibly registers or local vars)
    STACK_TOP
    */

    // EBP for this function
    uint32 ebp = debug_getreg_ebp();

    Debug::PrintCallStack(num_items, ebp);
}

void Debug::PrintCallStack(uint32 num_items, uint32 ebp)
{
    uint32 stack_top = (uint32)Stack::GetTopAddress();

    uint32 *val = (uint32 *)ebp;

    bool prev_ebp = false;

    for (uint32 i = 0; i < num_items && (uint32)val < stack_top; i++)
    {
        uint32 i_ptr = (uint32)val;
        uint32 i_val = *val;

        if (prev_ebp)
        {
            Debug::WriteSymbol(i_val);
            Terminal::Write(" at 0x");
            Terminal::Write(i_val);
            Terminal::WriteLine();
        }

        if (i_ptr == ebp)
        {
            ebp = i_val;
            prev_ebp = true;
        }
        else
        {
            prev_ebp = false;
        }

        val++;
    }
}

void Debug::WriteSymbol()
{
    // Looks like the call instruction to this symbol is 5 bytes, so subtract 5 to get the address that called this function
    uint32 eip = (uint32)__builtin_return_address(0) - 5;
    Debug::WriteSymbol(eip);
}

void Debug::WriteSymbol(uint32 eip)
{
    elf32_symtab_entry *symbol = Debug::GetSymbolFromAddress(eip);
    uint32 strtab = (uint32)MultibootInfo::GetElfStringTable()->sh_addr;
    Terminal::Write((string)(strtab + symbol->st_name));
    Terminal::Write("+0x");
    Terminal::Write((uint16)(eip - symbol->st_value));
}