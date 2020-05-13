#include "boot.h"
#include "debug.h"
#include "terminal.h"

void dbg_print_memory(void *ptr, uint32 num_bytes)
{
    if (num_bytes % 16 != 0)
    {
        num_bytes = ((num_bytes / 16) + 1) * 16;
    }

    volatile uint8 *val = ptr;

    terminal_write("Memory contents beginning at 0x");
    terminal_write_uint32((uint32)ptr);
    terminal_writeline(":");
    terminal_writeline("");

    uint8 nybble = (uint8)((uint32)ptr & 0x0000000F);
    terminal_write("                 ");

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
    terminal_writeline("                ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿");

    for (int i = 0; i < num_bytes; i++)
    {
        if (i % 16 == 0)
        {
            if (i != 0)
            {
                terminal_writeline(" ³");
            }

            terminal_write("ADDR 0x");
            terminal_write_uint32((uint32)val);
            terminal_write(" ³");
        }

        terminal_write(" ");
        terminal_write_uint8(*val);
        val++;
    }

    terminal_writeline(" ³\n                ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ");
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

    terminal_write("EBP=0x");
    terminal_write_uint32(ebp);
    terminal_writeline("");

    volatile uint32 *val = (uint32 *)ebp;

    // Unwind current ebp to the caller's EBP
    ebp = *val;
    val += 3;

    terminal_write("EBP=0x");
    terminal_write_uint32(ebp);
    terminal_writeline("");

    terminal_write("Stack contents beginning at 0x");
    terminal_write_uint32((uint32)val);
    terminal_writeline(":");

    // Track if the previous stack item was ebp
    bool prev_ebp = FALSE;

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
            prev_ebp = TRUE;
        }
        else
        {
            prev_ebp = FALSE;
        }

        terminal_writeline("");

        val++;
    }

    if ((uint32)val >= (uint32)stack_top)
    {
        terminal_writeline("  <<TOP OF STACK>>");
    }
}