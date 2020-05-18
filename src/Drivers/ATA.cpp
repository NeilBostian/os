#include <Drivers/ATA.h>
#include <Drivers/Serial.h>
#include <Terminal.h>

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
    return Serial::InB(PRIMARY_IO_7);
}

uint8 get_error_register()
{
    return Serial::InB(PRIMARY_IO_1);
}

void test_ata()
{
    //dump_regs_primary();

    // Drive Select
    Serial::OutB(PRIMARY_IO_6, 0x00);

    // isDMA = 0
    Serial::OutB(PRIMARY_IO_1, 0);
    Serial::OutB(PRIMARY_IO_4, 32);
    Serial::OutB(PRIMARY_IO_5, 0);

    // Command Opcode "PACKET"
    Serial::OutB(PRIMARY_IO_7, 0xA0);

    //dump_regs_primary();
}

bool first_interrupt = true;
void ata_handle_irq()
{
    if (first_interrupt)
    {
        dump_regs_primary();

        // Command stream
        Serial::OutB(PRIMARY_IO_7, 0x43);
        Serial::OutB(PRIMARY_IO_7, 0x00);

        Serial::OutB(PRIMARY_IO_7, 0x01);
        Serial::OutB(PRIMARY_IO_7, 0x00);

        Serial::OutB(PRIMARY_IO_7, 0x00);
        Serial::OutB(PRIMARY_IO_7, 0x00);

        Serial::OutB(PRIMARY_IO_7, 0x00);
        Serial::OutB(PRIMARY_IO_7, 0x00);

        Serial::OutB(PRIMARY_IO_7, 0x0C);
        Serial::OutB(PRIMARY_IO_7, 0x40);

        Serial::OutB(PRIMARY_IO_7, 0x00);
        Serial::OutB(PRIMARY_IO_7, 0x00);

        first_interrupt = false;
    }
    else
    {
        dump_regs_primary();
    }
}

void dump_regs_primary()
{
    Terminal::Write("ATA PIO Primary (");
    Terminal::Write((uint8)PRIMARY_IO_0);
    Terminal::Write(") Registers:");
    Terminal::WriteLine();

    uint8 b;

    Terminal::Write("Data     (+0) 0x");
    b = Serial::InB(PRIMARY_IO_0);
    Terminal::Write(b);
    Terminal::Write(", 0b");
    Terminal::WriteBin(b);
    Terminal::WriteLine();

    Terminal::Write("Error    (+1) 0x");
    b = Serial::InB(PRIMARY_IO_1);
    Terminal::Write(b);
    Terminal::Write(", 0b");
    Terminal::WriteBin(b);
    Terminal::WriteLine();

    Terminal::Write("Sector   (+2) 0x");
    b = Serial::InB(PRIMARY_IO_2);
    Terminal::Write(b);
    Terminal::Write(", 0b");
    Terminal::WriteBin(b);
    Terminal::WriteLine();

    Terminal::Write("Sector # (+3) 0x");
    b = Serial::InB(PRIMARY_IO_3);
    Terminal::Write(b);
    Terminal::Write(", 0b");
    Terminal::WriteBin(b);
    Terminal::Write("");

    Terminal::Write("Cyl Low  (+4) 0x");
    b = Serial::InB(PRIMARY_IO_4);
    Terminal::Write(b);
    Terminal::Write(", 0b");
    Terminal::WriteBin(b);
    Terminal::WriteLine();

    Terminal::Write("Cyl High (+5) 0x");
    b = Serial::InB(PRIMARY_IO_5);
    Terminal::Write(b);
    Terminal::Write(", 0b");
    Terminal::WriteBin(b);
    Terminal::WriteLine();

    Terminal::Write("Drive    (+6) 0x");
    b = Serial::InB(PRIMARY_IO_6);
    Terminal::Write(b);
    Terminal::Write(", 0b");
    Terminal::WriteBin(b);
    Terminal::WriteLine();

    Terminal::Write("Status   (+7) 0x");
    b = Serial::InB(PRIMARY_IO_7);
    Terminal::Write(b);
    Terminal::Write(", 0b");
    Terminal::WriteBin(b);
    Terminal::WriteLine();

    Terminal::Write("CTRL (CTRL+0) 0x");
    b = Serial::InB(PRIMARY_CMD_0);
    Terminal::Write(b);
    Terminal::Write(", 0b");
    Terminal::WriteBin(b);
    Terminal::WriteLine();

    Terminal::Write("Addr (CTRL+1) 0x");
    b = Serial::InB(PRIMARY_CMD_1);
    Terminal::Write(b);
    Terminal::Write(", 0b");
    Terminal::WriteBin(b);
    Terminal::WriteLine();
    Terminal::WriteLine();
}
