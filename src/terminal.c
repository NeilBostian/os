#include "terminal.h"

const int8 VGA_WIDTH = 80;
const int8 VGA_HEIGHT = 25;

const vga_color VGA_COLOR_BLACK = 0;
const vga_color VGA_COLOR_BLUE = 1;
const vga_color VGA_COLOR_GREEN = 2;
const vga_color VGA_COLOR_CYAN = 3;
const vga_color VGA_COLOR_RED = 4;
const vga_color VGA_COLOR_MAGENTA = 5;
const vga_color VGA_COLOR_BROWN = 6;
const vga_color VGA_COLOR_LIGHT_GREY = 7;
const vga_color VGA_COLOR_DARK_GREY = 8;
const vga_color VGA_COLOR_LIGHT_BLUE = 9;
const vga_color VGA_COLOR_LIGHT_GREEN = 10;
const vga_color VGA_COLOR_LIGHT_CYAN = 11;
const vga_color VGA_COLOR_LIGHT_RED = 12;
const vga_color VGA_COLOR_LIGHT_MAGENTA = 13;
const vga_color VGA_COLOR_LIGHT_BROWN = 14;
const vga_color VGA_COLOR_WHITE = 15;
const vga_color VGA_COLOR_DEFAULT = VGA_COLOR_LIGHT_GREY;

terminal_char *terminal_buffer = (terminal_char *)0xB8000;
static uint8 cursor_row = 0;
static uint8 cursor_col = 0;

static void terminal_putchar(char c);

int32 strlen(const char *str)
{
    int32 ret = 0;
    while (str[ret] != 0)
        ret++;
    return ret;
}

void terminal_clear()
{
    int row = 0,
        col = 0;

    terminal_char *buffer = terminal_buffer;

    for (row; row < VGA_HEIGHT; row++)
    {
        for (col; col < VGA_WIDTH; col++)
        {
            terminal_char t;
            t.value = ' ';
            t.color = VGA_COLOR_DEFAULT;

            *buffer = t;
            buffer++;
        }
    }
}

void terminal_write(char *str)
{
    int index = 0,
        len = strlen(str);

    for (index; index < len; index++)
    {
        terminal_putchar(str[index]);
    }
}

void terminal_write_uint8(uint8 x)
{
    char str[3];
    uint8_to_str(x, str);
    terminal_write(str);
}

void terminal_write_uint8bin(uint8 x)
{
    char str[9];
    uint8_to_strbin(x, str);
    terminal_write(str);
}

void terminal_write_uint32(uint32 x)
{
    char str[9];
    uint32_to_str(x, str);
    terminal_write(str);
}

void terminal_writeline(char *str)
{
    int index = 0,
        len = strlen(str);

    for (index; index < len; index++)
    {
        terminal_putchar(str[index]);
    }

    terminal_putchar('\n');
}

void terminal_putchar(char c)
{
    int loc;

    // Handle character output and terminal_row/column modification
    switch (c)
    {
    case '\n':
        cursor_row++;
        cursor_col = 0;
        break;

    case '\t':
        terminal_write("    ");
        break;

    default:
        loc = cursor_col + (cursor_row * VGA_WIDTH);
        terminal_buffer[loc].value = c;
        cursor_col++;
    }

    // Handle validation on cursor_col before terminal_row, since the logic in cursor_col can update terminal_row
    if (cursor_col >= VGA_WIDTH)
    {
        cursor_col = 0;
        cursor_row++;
    }

    // Handle validating terminal_row, and scrolling the screen upwards if necessary.
    if (cursor_row >= VGA_HEIGHT)
    {
        int col, row;
        for (col = 0; col < VGA_WIDTH; col++)
        {
            for (row = 0; row < VGA_HEIGHT - 1; row++)
            {
                int curInd = (row * VGA_WIDTH) + col;
                int targetInd = ((row + 1) * VGA_WIDTH) + col;
                terminal_buffer[curInd] = terminal_buffer[targetInd];
            }
        }

        // Also clear out the bottom row
        for (col = 0; col < VGA_WIDTH; col++)
        {
            loc = col + ((VGA_HEIGHT - 1) * VGA_WIDTH);
            terminal_buffer[loc].value = ' ';
        }

        cursor_row = VGA_HEIGHT - 1;
    }
}

void uint8_to_str(uint8 x, char *res)
{
    char as32[9];
    uint32_to_str((uint32)x, as32);
    res[0] = as32[6];
    res[1] = as32[7];
    res[2] = '\0';
}

void uint8_to_strbin(uint8 x, char *res)
{
    int i = 0;
    for (; i < 8; i++)
    {
        uint8 mask = 1 << i;
        if (x & mask)
        {
            res[7 - i] = '1';
        }
        else
        {
            res[7 - i] = '0';
        }
    }
    res[8] = '\0';
}

void uint32_to_str(uint32 x, char *res)
{
    int i = 0;
    for (; i < 8; i++)
    {
        char nybble = (x & (0xF << 4 * (8 - i - 1))) >> 4 * (8 - i - 1);
        if (nybble <= 9)
        {
            res[i] = '0' + nybble;
        }
        else
        {
            res[i] = 'A' + nybble - 10;
        }
    }
    res[8] = '\0';
}