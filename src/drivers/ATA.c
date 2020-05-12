#include "ATA.h"
#include "serial.h"
#include "terminal.h"

// See https://wiki.osdev.org/ATA_PIO_Mode#Registers
#define PRIMARY_IO_0 0x1F0
#define PRIMARY_IO_1 PRIMARY_IO_0 + 1
#define PRIMARY_IO_2 PRIMARY_IO_0 + 2
#define PRIMARY_IO_3 PRIMARY_IO_0 + 3
#define PRIMARY_IO_4 PRIMARY_IO_0 + 4
#define PRIMARY_IO_5 PRIMARY_IO_0 + 5
#define PRIMARY_IO_6 PRIMARY_IO_0 + 6
#define PRIMARY_IO_7 PRIMARY_IO_0 + 7
#define PRIMARY_CMD_0 0x3F6
#define PRIMARY_CMD_1 PRIMARY_CMD_0 + 1

#define SECONDARY_IO_0 0x170
#define SECONDARY_IO_1 SECONDARY_IO_0 + 1
#define SECONDARY_IO_2 SECONDARY_IO_0 + 2
#define SECONDARY_IO_3 SECONDARY_IO_0 + 3
#define SECONDARY_IO_4 SECONDARY_IO_0 + 4
#define SECONDARY_IO_5 SECONDARY_IO_0 + 5
#define SECONDARY_IO_6 SECONDARY_IO_0 + 6
#define SECONDARY_IO_7 SECONDARY_IO_0 + 7
#define SECONDARY_CMD_0 0x376
#define SECONDARY_CMD_1 SECONDARY_CMD_0 + 1

static void dump_regs_primary();

uint8 get_status_register()
{
    return inb(PRIMARY_IO_7);
}

uint8 get_error_register()
{
    return inb(PRIMARY_IO_1);
}

void test_atapio()
{
    dump_regs_primary();

    // Drive Select
    outb(PRIMARY_IO_6, 0x00);


    // isDMA = 1
    outb(PRIMARY_IO_1, 1);
    outb(PRIMARY_IO_4, 0);
    outb(PRIMARY_IO_5, 0);

    // Command Opcode "PACKET"
    outb(PRIMARY_IO_7, 0xA0);

    // Command stream
    outb(PRIMARY_IO_7, 0x43);
    outb(PRIMARY_IO_7, 0x00);

    outb(PRIMARY_IO_7, 0x01);
    outb(PRIMARY_IO_7, 0x00);

    outb(PRIMARY_IO_7, 0x00);
    outb(PRIMARY_IO_7, 0x00);

    outb(PRIMARY_IO_7, 0x00);
    outb(PRIMARY_IO_7, 0x00);

    outb(PRIMARY_IO_7, 0x0C);
    outb(PRIMARY_IO_7, 0x40);

    outb(PRIMARY_IO_7, 0x00);
    outb(PRIMARY_IO_7, 0x00);

    dump_regs_primary();
}

void dump_regs_primary()
{
    terminal_write("ATA PIO Primary (");
    terminal_write_uint8((uint8)PRIMARY_IO_0);
    terminal_write(") Registers:");
    terminal_writeline("");

    uint8 b;

    terminal_write("Data     (+0) 0x");
    b = inb(PRIMARY_IO_0);
    terminal_write_uint8(b);
    terminal_write(", 0b");
    terminal_write_uint8bin(b);
    terminal_writeline("");

    terminal_write("Error    (+1) 0x");
    b = inb(PRIMARY_IO_1);
    terminal_write_uint8(b);
    terminal_write(", 0b");
    terminal_write_uint8bin(b);
    terminal_writeline("");

    terminal_write("Sector   (+2) 0x");
    b = inb(PRIMARY_IO_2);
    terminal_write_uint8(b);
    terminal_write(", 0b");
    terminal_write_uint8bin(b);
    terminal_writeline("");

    terminal_write("Sector # (+3) 0x");
    b = inb(PRIMARY_IO_3);
    terminal_write_uint8(b);
    terminal_write(", 0b");
    terminal_write_uint8bin(b);
    terminal_writeline("");

    terminal_write("Cyl Low  (+4) 0x");
    b = inb(PRIMARY_IO_4);
    terminal_write_uint8(b);
    terminal_write(", 0b");
    terminal_write_uint8bin(b);
    terminal_writeline("");

    terminal_write("Cyl High (+5) 0x");
    b = inb(PRIMARY_IO_5);
    terminal_write_uint8(b);
    terminal_write(", 0b");
    terminal_write_uint8bin(b);
    terminal_writeline("");

    terminal_write("Drive    (+6) 0x");
    b = inb(PRIMARY_IO_6);
    terminal_write_uint8(b);
    terminal_write(", 0b");
    terminal_write_uint8bin(b);
    terminal_writeline("");

    terminal_write("Status   (+7) 0x");
    b = inb(PRIMARY_IO_7);
    terminal_write_uint8(b);
    terminal_write(", 0b");
    terminal_write_uint8bin(b);
    terminal_writeline("");

    terminal_write("CTRL (CTRL+0) 0x");
    b = inb(PRIMARY_CMD_0);
    terminal_write_uint8(b);
    terminal_write(", 0b");
    terminal_write_uint8bin(b);
    terminal_writeline("");

    terminal_write("Addr (CTRL+1) 0x");
    b = inb(PRIMARY_CMD_1);
    terminal_write_uint8(b);
    terminal_write(", 0b");
    terminal_write_uint8bin(b);
    terminal_writeline("");
    terminal_writeline("");
}
