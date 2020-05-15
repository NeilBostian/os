#include "debug.h"
#include "terminal.h"

// Defined in boot.cpp
extern void *stack_bottom;
extern void *stack_top;

static string readable_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`~-_=+!@#$%^&*()[{]};:'\",<.>/?\\|";

void dbg_print_memory(void *ptr, uint32 num_bytes)
{
    if (num_bytes % 16 != 0)
    {
        num_bytes = ((num_bytes / 16) + 1) * 16;
    }

    uint8 *val = (uint8 *)ptr;

    terminal_write("Memory contents beginning at 0x");
    terminal_write_uint32((uint32)ptr);
    terminal_writeline(":");
    terminal_writeline("");

    uint8 nybble = (uint8)((uint32)ptr & 0x0000000F);
    terminal_write("           ");

    char chr[3];
    for (int i = 0; i < 16; i++)
    {
        terminal_write(" ");
        uint8_to_str(nybble, chr);
        chr[0] = 'x';
        terminal_write(chr);

        nybble = (nybble + 1) % 16;
    }
    terminal_writeline("");
    terminal_writeline("           ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿");

    uint32 print_chars_len = strlen(readable_chars);
    for (int i = 0; i < num_bytes; i++)
    {
        if (i % 16 == 0)
        {
            if (i != 0)
            {
                terminal_write("³ ");

                for (int j = i - 16; j < i; j++)
                {
                    char *base = (char *)ptr;
                    char c = base[j];

                    bool did_print = false;
                    for (int k = 0; k < print_chars_len; k++)
                    {
                        if (readable_chars[k] == c)
                        {
                            terminal_putchar(c, VGA_FG_LIGHT_GREY);
                            did_print = true;
                            break;
                        }
                    }

                    if (!did_print)
                    {
                        terminal_putchar('.', VGA_FG_DARK_GREY);
                    }
                }
                terminal_writeline("");
            }

            terminal_write("0x");
            terminal_write_uint32((uint32)val);
            terminal_write(" ³");
        }
        else
        {
            terminal_write(" ");
        }

        uint8 x = *val;
        uint8_to_str(x, chr);
        vga_color color = (x == 0 || x == 0xFF ? VGA_FG_DARK_GREY : VGA_FG_WHITE);

        terminal_putchar(chr[0], color);
        terminal_putchar(chr[1], color);

        val++;
    }

    terminal_write("³ ");

    for (int j = num_bytes - 16; j < num_bytes; j++)
    {
        char *base = (char *)ptr;
        char c = base[j];

        bool did_print = false;
        for (int k = 0; k < print_chars_len; k++)
        {
            if (readable_chars[k] == c)
            {
                terminal_putchar(c, VGA_FG_LIGHT_GREY);
                did_print = true;
                break;
            }
        }

        if (!did_print)
        {
            terminal_putchar('.', VGA_FG_DARK_GREY);
        }
    }
    terminal_writeline("");
    terminal_writeline("           ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ");
}

void dbg_print_stack(uint32 num_items)
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
    uint32 ebp = dbg_getreg_ebp();

    volatile uint32 *val = (uint32 *)ebp;

    // Unwind current ebp to the caller's EBP
    ebp = *val;
    val += 3;

    terminal_write("Stack contents beginning at 0x");
    terminal_write_uint32((uint32)val);
    terminal_writeline(":");

    // Track if the previous stack item was ebp
    bool prev_ebp = false;

    for (uint32 i = 0; i < num_items && (uint32)val < (uint32)stack_top; i++)
    {
        uint32 i_ptr = (uint32)val;
        uint32 i_val = *val;

        terminal_write("  ADDR 0x");
        terminal_write_uint32(i_ptr);
        terminal_write(": 0x");
        terminal_write_uint32(i_val);

        if (prev_ebp)
        {
            terminal_write(" <-- RET ADDRESS");
        }

        if (i_ptr == ebp)
        {
            terminal_write(" <-- EBP");
            ebp = i_val;
            prev_ebp = true;
        }
        else
        {
            prev_ebp = false;
        }

        terminal_writeline("");

        val++;
    }

    if ((uint32)val >= (uint32)stack_top)
    {
        terminal_writeline("  <<TOP OF STACK>>");
    }
}