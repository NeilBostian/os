#include "serial.h"

// See https://wiki.osdev.org/Inline_Assembly/Examples#I.2FO_access
inline void outb(uint16 port, uint8 val)
{
    asm volatile("outb %0, %1" ::"a"(val), "Nd"(port));
}

inline uint8 inb(uint16 port)
{
    uint8 ret;

    asm volatile("inb %1, %0"
                 : "=a"(ret)
                 : "Nd"(port));

    return ret;
}