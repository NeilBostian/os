#include "terminal.h"

const size_t VGA_WIDTH = 80;
const size_t VGA_HEIGHT = 24;

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

size_t strlen(const char *str)
{
    size_t ret = 0;
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
        terminal_char t;
        t.value = str[index];
        t.color = VGA_COLOR_DEFAULT;

        terminal_buffer[index] = t;
    }
}