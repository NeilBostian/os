#ifndef TERMINAL_H
#define TERMINAL_H

#include "types.h"

const int8 VGA_WIDTH;
const int8 VGA_HEIGHT;

typedef uint8 vga_color;

typedef struct
{
    char value;
    vga_color color;
} __attribute__((packed)) terminal_char;

const vga_color VGA_COLOR_BLACK;
const vga_color VGA_COLOR_BLUE;
const vga_color VGA_COLOR_GREEN;
const vga_color VGA_COLOR_CYAN;
const vga_color VGA_COLOR_RED;
const vga_color VGA_COLOR_MAGENTA;
const vga_color VGA_COLOR_BROWN;
const vga_color VGA_COLOR_LIGHT_GREY;
const vga_color VGA_COLOR_DARK_GREY;
const vga_color VGA_COLOR_LIGHT_BLUE;
const vga_color VGA_COLOR_LIGHT_GREEN;
const vga_color VGA_COLOR_LIGHT_CYAN;
const vga_color VGA_COLOR_LIGHT_RED;
const vga_color VGA_COLOR_LIGHT_MAGENTA;
const vga_color VGA_COLOR_LIGHT_BROWN;
const vga_color VGA_COLOR_WHITE;
const vga_color VGA_COLOR_DEFAULT;

void terminal_clear();
void terminal_write(char *str);
void terminal_write_uint8(uint8 x);
void terminal_write_uint32(uint32 x);
void terminal_writeline(char *str);
void uint8_to_str(uint8 x, char* res);
void uint32_to_str(uint32 x, char *res);

#endif
