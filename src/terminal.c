#include "terminal.h"
#include "cpu.h"

// Number of characters in a row
#define VGA_WIDTH 80

// VGA_HEIGHT is actually 25, but this terminal uses the bottom row for static information
#define VGA_HEIGHT 24

// Maximum amount of characters in the history array
#define HIST_SIZE 1024 * 1024 * 4

// Upper bound of cursor_row based on HIST_SIZE
#define MAX_ROW HIST_SIZE / VGA_WIDTH

// Upper bound of current_row_offset based on HIST_SIZE / VGA_HEIGHT
#define MAX_ROW_OFFSET MAX_ROW - VGA_HEIGHT

terminal_char *terminal_buffer = (terminal_char *)0xB8000;
static uint32 cursor_row = 0;
static uint32 cursor_col = 0;
static terminal_char terminal_history[HIST_SIZE];
static int current_row_offset = 0;

static void map_history_to_buffer(uint32 row_offset);

int32 strlen(const char *str)
{
    int32 ret = 0;
    while (str[ret] != 0)
        ret++;
    return ret;
}

void terminal_clear()
{
    cursor_row = 0;
    cursor_col = 0;

    for (uint32 row = 0; row < VGA_HEIGHT; row++)
    {
        for (uint32 col = 0; col < VGA_WIDTH; col++)
        {
            uint32 offset = col + row * VGA_HEIGHT;
            terminal_buffer[offset].value = ' ';
            terminal_buffer[offset].color = VGA_COLOR_DEFAULT;
        }
    }

    for (uint32 hist_ind = 0; hist_ind < HIST_SIZE; hist_ind++)
    {
        terminal_history[hist_ind].value = ' ';
        terminal_history[hist_ind].color = VGA_COLOR_DEFAULT;
    }

    uint32 lastrow_offset = 24 * VGA_WIDTH;
    for (uint32 ind = 0; ind < VGA_WIDTH; ind++)
    {
        terminal_buffer[lastrow_offset + ind].color = VGA_FG_BLACK | VGA_BG_WHITE;
    }

    char *msg = " Scroll (arrow u/d = 1 line, pg up/down = 10 lines, home/end)";
    uint32 len = strlen(msg);
    for (uint32 ind = 0; ind < len; ind++)
    {
        uint32 o = lastrow_offset + ind;
        terminal_buffer[o].value = msg[ind];
    }
}

void terminal_write(char *str)
{
    int index = 0,
        len = strlen(str);

    for (index; index < len; index++)
    {
        terminal_putchar(str[index], VGA_COLOR_DEFAULT);
    }

    map_history_to_buffer(current_row_offset);
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
        terminal_putchar(str[index], VGA_COLOR_DEFAULT);
    }

    terminal_putchar('\n', VGA_COLOR_DEFAULT);

    map_history_to_buffer(current_row_offset);
}

void terminal_putchar(char c, vga_color color)
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
        terminal_history[loc].value = c;
        terminal_history[loc].color = color;
        cursor_col++;
    }

    // Handle validation on cursor_col before terminal_row, since the logic in cursor_col can update terminal_row
    if (cursor_col >= VGA_WIDTH)
    {
        cursor_col = 0;
        cursor_row++;
    }

    if (cursor_row - current_row_offset >= VGA_HEIGHT)
    {
        current_row_offset = cursor_row - VGA_HEIGHT;

        if (current_row_offset >= MAX_ROW_OFFSET)
        {
            current_row_offset = MAX_ROW_OFFSET - 1;
        }
    }

    if (cursor_row >= MAX_ROW)
    {
        panic();
    }
}

void terminal_pagetop()
{
    terminal_pageup(MAX_ROW_OFFSET);
}
void terminal_pageup(uint32 offset)
{
    current_row_offset -= offset;

    if (current_row_offset < 0)
    {
        current_row_offset = 0;
    }

    map_history_to_buffer(current_row_offset);
}

void terminal_pagebottom()
{
    terminal_pagedown(MAX_ROW_OFFSET);
}
void terminal_pagedown(uint32 offset)
{
    uint32 max_offset = cursor_row - VGA_HEIGHT;

    if (max_offset >= MAX_ROW_OFFSET)
    {
        max_offset = MAX_ROW_OFFSET - 1;
    }

    current_row_offset += offset;

    if (current_row_offset < 0)
    {
        current_row_offset = 0;
    }
    else if (current_row_offset >= max_offset)
    {
        current_row_offset = max_offset;
    }

    map_history_to_buffer(current_row_offset);
}

void map_history_to_buffer(uint32 row_offset)
{
    uint32 hist_offset = row_offset * VGA_WIDTH;

    for (uint32 row = 0; row < VGA_HEIGHT; row++)
    {
        for (uint32 col = 0; col < VGA_WIDTH; col++)
        {
            uint32 offset = col + row * VGA_WIDTH;
            terminal_buffer[offset].value = terminal_history[hist_offset + offset].value;
            terminal_buffer[offset].color = terminal_history[hist_offset + offset].color;
        }
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