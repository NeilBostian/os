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

typedef struct
{
    char value;
    vga_color color;
} __attribute__((packed)) terminal_char;

terminal_char *terminal_buffer = (terminal_char *)0xB8000;
static uint32 cursor_row = 0;
static uint32 cursor_col = 0;
static terminal_char terminal_history[HIST_SIZE];
static int current_row_offset = 0;

void Terminal::Clear()
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

    string msg = " Scroll (arrow u/d = 1 line, pg up/down = 10 lines, home/end)";
    uint32 len = String::Len(msg);
    for (uint32 ind = 0; ind < len; ind++)
    {
        uint32 o = lastrow_offset + ind;
        terminal_buffer[o].value = msg[ind];
    }
}

void Terminal::Write(string str)
{
    Terminal::Write(str, VGA_COLOR_DEFAULT);
}
void Terminal::Write(string str, vga_color color)
{
    int index = 0,
        len = String::Len(str);

    for (index; index < len; index++)
    {
        Terminal::PutChar(str[index], color);
    }

    Terminal::MapHistoryToBuffer(current_row_offset);
}

void Terminal::Write(char c)
{
    Terminal::PutChar(c, VGA_COLOR_DEFAULT);
}

void Terminal::Write(char c, vga_color color)
{
    Terminal::PutChar(c, color);
}

void Terminal::Write(uint8 x)
{
    char str[3];
    String::ConvertToHex(x, str);
    Terminal::Write(str);
}

void Terminal::WriteBin(uint8 x)
{
    char str[9];
    String::ConvertToBin(x, str);
    Terminal::Write(str);
}

void Terminal::Write(uint32 x)
{
    char str[9];
    String::ConvertToHex(x, str);
    Terminal::Write(str);
}

void Terminal::WriteLine(string str)
{
    int index = 0,
        len = String::Len(str);

    for (index; index < len; index++)
    {
        Terminal::PutChar(str[index], VGA_COLOR_DEFAULT);
    }

    Terminal::PutChar('\n', VGA_COLOR_DEFAULT);

    Terminal::MapHistoryToBuffer(current_row_offset);
}

void Terminal::PutChar(char c, vga_color color)
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
        Terminal::Write("    ");
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

void Terminal::PageTop()
{
    Terminal::PageUp(MAX_ROW_OFFSET);
}

void Terminal::PageUp(uint32 offset)
{
    current_row_offset -= offset;

    if (current_row_offset < 0)
    {
        current_row_offset = 0;
    }

    Terminal::MapHistoryToBuffer(current_row_offset);
}

void Terminal::PageBottom()
{
    Terminal::PageDown(MAX_ROW_OFFSET);
}

void Terminal::PageDown(uint32 offset)
{
    uint32 max_offset;

    if (cursor_row < VGA_HEIGHT)
    {
        max_offset = 0;
    }
    else
    {
        max_offset = cursor_row - VGA_HEIGHT;
    }

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

    Terminal::MapHistoryToBuffer(current_row_offset);
}

void Terminal::MapHistoryToBuffer(uint32 rowOffset)
{
    uint32 hist_offset = rowOffset * VGA_WIDTH;

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
