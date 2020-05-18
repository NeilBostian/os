#include <Drivers/Serial.h>

// See https://wiki.osdev.org/Inline_Assembly/Examples#I.2FO_access

void Serial::OutB(SerialPort port, uint8 val)
{
    asm volatile("outb %0, %1" ::"a"(val), "Nd"(port));
}

void Serial::OutW(SerialPort port, uint16 val)
{
    asm volatile("outw %0, %1" ::"a"(val), "Nd"(port));
}

uint8 Serial::InB(SerialPort port)
{
    uint8 ret;

    asm volatile("inb %1, %0"
                 : "=a"(ret)
                 : "Nd"(port));

    return ret;
}

uint16 Serial::InW(SerialPort port)
{
    uint16 ret;

    asm volatile("inw %1, %0"
                 : "=a"(ret)
                 : "Nd"(port));

    return ret;
}