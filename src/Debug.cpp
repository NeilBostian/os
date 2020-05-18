#include <Cpu/Stack.h>
#include <Debug.h>
#include <Terminal.h>

// Returns a pointer to the next address that will be executed after this function is called.
// AKA - returns the return address of this function
extern "C" uint32 debug_getreg_eip();

// Returns register %ebp
extern "C" uint32 debug_getreg_ebp();

static string readable_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`~-_=+!@#$%^&*()[{]};:'\",<.>/?\\|";

uint32 Debug::GetRegEBP()
{
    return debug_getreg_ebp();
}

uint32 Debug::GetRegEIP()
{
    return debug_getreg_eip();
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
    uint32 ebp = Debug::GetRegEBP();

    volatile uint32 *val = (uint32 *)ebp;

    // Unwind current ebp to the caller's EBP
    ebp = *val;
    val += 3;

    Terminal::Write("Stack contents beginning at 0x");
    Terminal::Write((uint32)val);
    Terminal::WriteLine(":");

    // Track if the previous stack item was ebp
    bool prev_ebp = false;

    uint32 stack_top = (uint32)Stack::GetTopAddress();
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
            Terminal::Write(" <-- RET ADDRESS");
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

        Terminal::WriteLine();

        val++;
    }

    if ((uint32)val >= stack_top)
    {
        Terminal::WriteLine("  <<TOP OF STACK>>");
    }
}
