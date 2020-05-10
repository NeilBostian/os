#ifndef TERMINAL_H
#define TERMINAL_H

#include <stddef.h>

const size_t VGA_WIDTH;
const size_t VGA_HEIGHT;

typedef unsigned char vga_color;

typedef struct
{
    char value;
    vga_color color;
} terminal_char;

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
void terminal_writeint(unsigned int x);

#endif