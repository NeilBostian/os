#include <Terminal.h>
#include <Cpu/Cpu.h>

// Number of characters in a row
#define VGA_WIDTH 80

// VGA_HEIGHT is actually 25, but this terminal uses the bottom row for static information
#define VGA_HEIGHT 24

// Maximum amount of characters in the history array
#define HIST_SIZE 1024 * 1024 * 4

// Upper bound of cursor_row based on HIST_SIZE
#define MAX_ROW HIST_SIZE / VGA_WIDTH

// Upper bound of Terminal::CurrentRowOffset based on HIST_SIZE / VGA_HEIGHT
#define MAX_ROW_OFFSET MAX_ROW - VGA_HEIGHT

typedef struct
{
    char value;
    vga_color color;
} __attribute__((packed)) terminal_char;

terminal_char *terminal_buffer = (terminal_char *)0xB8000;
static terminal_char terminal_history[HIST_SIZE];

uint32 Terminal::CursorRow = 0;
uint32 Terminal::CursorCol = 0;
uint32 Terminal::CurrentRowOffset = 0;

void Terminal::Clear()
{
    Terminal::CursorRow = 0;
    Terminal::CursorCol = 0;

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

    Terminal::PageBottom();
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

void Terminal::Write(uint16 x)
{
    char str[5];
    String::ConvertToHex(x, str);
    Terminal::Write(str);
}

void Terminal::Write(uint32 x)
{
    char str[9];
    String::ConvertToHex(x, str);
    Terminal::Write(str);
}

void Terminal::WriteLine()
{
    Terminal::Write("\n");
}

void Terminal::WriteLine(string str)
{
    Terminal::WriteLine(str, VGA_COLOR_DEFAULT);
}

void Terminal::WriteLine(string str, vga_color color)
{
    int index = 0,
        len = String::Len(str);

    for (index; index < len; index++)
    {
        Terminal::PutChar(str[index], color);
    }

    Terminal::PutChar('\n', color);

    Terminal::PageBottom();
}

void Terminal::PutChar(char c, vga_color color)
{
    int loc;

    // Handle character output and terminal_row/column modification
    switch (c)
    {
    case '\n':
        Terminal::CursorRow++;
        Terminal::CursorCol = 0;
        break;

    case '\t':
        Terminal::Write("    ");
        break;

    default:
        loc = Terminal::CursorCol + (Terminal::CursorRow * VGA_WIDTH);
        terminal_history[loc].value = c;
        terminal_history[loc].color = color;
        Terminal::CursorCol++;
    }

    if (Terminal::CursorCol >= VGA_WIDTH)
    {
        Terminal::CursorCol = 0;
        Terminal::CursorRow++;
    }

    if (Terminal::CursorRow >= MAX_ROW)
    {
        Cpu::Panic();
    }
}

void Terminal::PageTop()
{
    Terminal::CurrentRowOffset = 0;
    Terminal::MapHistoryToBuffer(Terminal::CurrentRowOffset);
}

void Terminal::PageUp(uint32 offset)
{
    uint32 newOffset = Terminal::CurrentRowOffset - offset;

    if (newOffset > Terminal::CurrentRowOffset) // newOffset > Terminal::CurrentRowOffset occurs on integer overflow
    {
        newOffset = 0;
    }

    Terminal::CurrentRowOffset = newOffset;
    Terminal::MapHistoryToBuffer(Terminal::CurrentRowOffset);
}

void Terminal::PageBottom()
{
    if (Terminal::CursorRow >= VGA_HEIGHT)
    {
        Terminal::CurrentRowOffset = Terminal::CursorRow - VGA_HEIGHT + 1;
    }
    else
    {
        Terminal::CurrentRowOffset = 0;
    }

    Terminal::MapHistoryToBuffer(Terminal::CurrentRowOffset);
}

void Terminal::PageDown(uint32 offset)
{
    if (Terminal::CursorRow >= VGA_HEIGHT)
    {
        uint32 maxOffset = Terminal::CursorRow - VGA_HEIGHT + 1;
        uint32 currentOffset = Terminal::CurrentRowOffset;
        uint32 newOffset = Terminal::CurrentRowOffset + offset;

        if (newOffset < currentOffset || newOffset > maxOffset)
        {
            newOffset = maxOffset;
        }

        Terminal::CurrentRowOffset = newOffset;
    }
    else
    {
        Terminal::CurrentRowOffset = 0;
    }

    Terminal::MapHistoryToBuffer(Terminal::CurrentRowOffset);
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
