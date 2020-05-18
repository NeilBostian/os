#pragma once

#include "types.h"

#define VGA_COLOR_BLACK 0
#define VGA_COLOR_BLUE 1
#define VGA_COLOR_GREEN 2
#define VGA_COLOR_CYAN 3
#define VGA_COLOR_RED 4
#define VGA_COLOR_MAGENTA 5
#define VGA_COLOR_BROWN 6
#define VGA_COLOR_LIGHT_GREY 7
#define VGA_COLOR_DARK_GREY 8
#define VGA_COLOR_LIGHT_BLUE 9
#define VGA_COLOR_LIGHT_GREEN 10
#define VGA_COLOR_LIGHT_CYAN 11
#define VGA_COLOR_LIGHT_RED 12
#define VGA_COLOR_LIGHT_MAGENTA 13
#define VGA_COLOR_LIGHT_BROWN 14
#define VGA_COLOR_WHITE 15

#define VGA_FG_BLACK VGA_COLOR_BLACK
#define VGA_FG_BLUE VGA_COLOR_BLUE
#define VGA_FG_GREEN VGA_COLOR_GREEN
#define VGA_FG_CYAN VGA_COLOR_CYAN
#define VGA_FG_RED VGA_COLOR_RED
#define VGA_FG_MAGENTA VGA_COLOR_MAGENTA
#define VGA_FG_BROWN VGA_COLOR_BROWN
#define VGA_FG_LIGHT_GREY VGA_COLOR_LIGHT_GREY
#define VGA_FG_DARK_GREY VGA_COLOR_DARK_GREY
#define VGA_FG_LIGHT_BLUE VGA_COLOR_LIGHT_BLUE
#define VGA_FG_LIGHT_GREEN VGA_COLOR_LIGHT_GREEN
#define VGA_FG_LIGHT_CYAN VGA_COLOR_LIGHT_CYAN
#define VGA_FG_LIGHT_RED VGA_COLOR_LIGHT_RED
#define VGA_FG_LIGHT_MAGENTA VGA_COLOR_LIGHT_MAGENTA
#define VGA_FG_LIGHT_BROWN VGA_COLOR_LIGHT_BROWN
#define VGA_FG_WHITE VGA_COLOR_WHITE

#define VGA_BG_BLACK VGA_COLOR_BLACK << 4
#define VGA_BG_BLUE VGA_COLOR_BLUE << 4
#define VGA_BG_GREEN VGA_COLOR_GREEN << 4
#define VGA_BG_CYAN VGA_COLOR_CYAN << 4
#define VGA_BG_RED VGA_COLOR_RED << 4
#define VGA_BG_MAGENTA VGA_COLOR_MAGENTA << 4
#define VGA_BG_BROWN VGA_COLOR_BROWN << 4
#define VGA_BG_LIGHT_GREY VGA_COLOR_LIGHT_GREY << 4
#define VGA_BG_DARK_GREY VGA_COLOR_DARK_GREY << 4
#define VGA_BG_LIGHT_BLUE VGA_COLOR_LIGHT_BLUE << 4
#define VGA_BG_LIGHT_GREEN VGA_COLOR_LIGHT_GREEN << 4
#define VGA_BG_LIGHT_CYAN VGA_COLOR_LIGHT_CYAN << 4
#define VGA_BG_LIGHT_RED VGA_COLOR_LIGHT_RED << 4
#define VGA_BG_LIGHT_MAGENTA VGA_COLOR_LIGHT_MAGENTA << 4
#define VGA_BG_LIGHT_BROWN VGA_COLOR_LIGHT_BROWN << 4
#define VGA_BG_WHITE VGA_COLOR_WHITE << 4

#define VGA_COLOR_DEFAULT VGA_FG_LIGHT_GREY | VGA_BG_BLACK

typedef uint8 vga_color;

class Terminal
{
public:
    static void Clear();

    static void WriteBin(uint8 x);

    static void Write(char c);
    static void Write(char c, vga_color color);
    static void Write(uint8 x);
    static void Write(uint32 x);
    static void Write(string str);
    static void Write(string str, vga_color color);

    static void WriteLine(string str);

    static void PageTop();
    static void PageUp(uint32 offset);
    static void PageBottom();
    static void PageDown(uint32 offset);

private:
    static uint32 CursorRow;
    static uint32 CursorCol;
    static uint32 CurrentRowOffset;
    static void PutChar(char c, vga_color color);
    static void MapHistoryToBuffer(uint32 rowOffset);
};