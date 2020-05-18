#include <Cpu.h>
#include <Debug.h>
#include <Drivers/ATA.h>
#include <Drivers/Serial.h>
#include <Interrupts.h>
#include <Terminal.h>
#include <Types.h>

#define PIC_ISR_OFFSET 0x20
#define ISR_KEYBOARD PIC_ISR_OFFSET + 1
#define ISR_ATA_PRIMARY PIC_ISR_OFFSET + 14
#define ISR_KERNEL_PANIC 0x30

#define PIC1_CMD 0x20
#define PIC2_CMD 0xA0
#define PIC1_DATA 0x21
#define PIC2_DATA 0xA1

#define ICW1_ICW4 0x01      /* ICW4 (not) needed */
#define ICW1_SINGLE 0x02    /* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04 /* Call address interval 4 (8) */
#define ICW1_LEVEL 0x08     /* Level triggered (edge) mode */
#define ICW1_INIT 0x10      /* Initialization - required! */

#define ICW4_8086 0x01       /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO 0x02       /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE 0x08  /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C /* Buffered mode/master */
#define ICW4_SFNM 0x10       /* Special fully nested (not) */

typedef struct
{
    uint16 limit_0_15; // bits 0-15 of limit
    uint16 base_0_15;
    uint8 base_16_23;
    uint8 access_byte;
    uint8 flags_and_limit_16_19;
    uint8 base_24_31;
} __attribute__((packed)) gdt_entry;

typedef struct
{
    uint16 limit;
    gdt_entry *address;
} __attribute__((packed)) gdt_descriptor;

typedef struct
{
    uint16 offset_1; // offset bits 0..15
    uint16 selector; // a code segment selector in GDT or LDT
    uint8 zero;      // unused, set to 0
    uint8 type_attr; // type and attributes, see below
    uint16 offset_2; // offset bits 16..31
} __attribute__((packed)) idt_entry;

typedef struct
{
    uint16 limit;
    idt_entry *address;
} __attribute__((packed)) idt_descriptor;

static gdt_entry gdt[5];
static gdt_descriptor gdt_pointer;

static idt_entry idt[64];
static idt_descriptor idt_pointer;

static void PIC_sendEOI(uint8 irq);
static void PIC_remap(uint8 offset);
static void PIC_disable_irq(uint8 irq);
static void PIC_enable_irq(uint8 irq);

extern "C" void cpu_reload_segments();

extern "C" void cpu_interrupt_handler(registers cpu, uint32 isr, uint32 error_code, uint32 eip)
{
    Cpu::HandleInterrupt(cpu, isr, error_code, eip);
}

void Cpu::Initialize()
{
    Interrupts::InitializeAllInterruptHandlers();

    Cpu::InitGdt();
    Cpu::InitIdt();
}

void Cpu::InitGdt()
{
    gdt_pointer.limit = sizeof(gdt) - 1;
    gdt_pointer.address = gdt;

    /* Kernel code segment */
    gdt[1].limit_0_15 = 0xFFFF;
    gdt[1].base_0_15 = 0x0000;
    gdt[1].base_16_23 = 0x00;
    gdt[1].access_byte = 0b10011010;
    gdt[1].flags_and_limit_16_19 = 0xCF;
    gdt[1].base_24_31 = 0x00;

    gdt[2].limit_0_15 = 0xFFFF;
    gdt[2].base_0_15 = 0x0000;
    gdt[2].base_16_23 = 0x00;
    gdt[2].access_byte = 0b10010010;
    gdt[2].flags_and_limit_16_19 = 0xCF;
    gdt[2].base_24_31 = 0x00;

    gdt[3].limit_0_15 = 0xFFFF;
    gdt[3].base_0_15 = 0x0000;
    gdt[3].base_16_23 = 0x00;
    gdt[3].access_byte = 0b11111010;
    gdt[3].flags_and_limit_16_19 = 0xCF;
    gdt[3].base_24_31 = 0x00;

    gdt[4].limit_0_15 = 0xFFFF;
    gdt[4].base_0_15 = 0x0000;
    gdt[4].base_16_23 = 0x00;
    gdt[4].access_byte = 0b11110010;
    gdt[4].flags_and_limit_16_19 = 0xCF;
    gdt[4].base_24_31 = 0x00;

    asm("lgdt %0" ::"m"(gdt_pointer)
        : "memory");

    cpu_reload_segments();
}

void Cpu::HandleInterrupt(registers cpu, uint32 isr, uint32 error_code, uint32 eip)
{
    if (isr == ISR_KEYBOARD)
    {
        uint8 scan_code = Serial::InB(0x60);

        switch (scan_code)
        {
        case 0x47: // Home
            Terminal::PageTop();
            break;
        case 0x48: // Arrow up
            Terminal::PageUp(1);
            break;
        case 0x49: // Page up
            Terminal::PageUp(10);
            break;
        case 0x4F: // End
            Terminal::PageBottom();
            break;
        case 0x50: // Arrow down
            Terminal::PageDown(1);
            break;
        case 0x51: // Page down
            Terminal::PageDown(10);
            break;
        }
    }
    else if (isr == ISR_KERNEL_PANIC)
    {
        Cpu::PanicInternal();
    }
    else if (isr == ISR_ATA_PRIMARY)
    {
        Terminal::WriteLine("Received ATA Primary Interrupt");
        ata_handle_irq();
    }
    else
    {
        Terminal::Write("Received interrupt 0x");
        Terminal::Write((uint8)isr);
        Terminal::WriteLine(", no handler configured for this ISR.");
    }

    if (isr >= PIC_ISR_OFFSET && isr < PIC_ISR_OFFSET + 16)
    {
        PIC_sendEOI(isr - PIC_ISR_OFFSET);
    }
}

void Cpu::InitIdt()
{
    // Interrupts 0-31,  reserved for CPU
    // Interrupts 32-47, reserved for PIC
    // Interrupt 48 - Kernel Panic
    // Interrupts 49-63, unused by kernel
    PIC_remap(PIC_ISR_OFFSET);

    // Only IRQ 1 (ISR 0x21) / keyboard enabled for now
    PIC_enable_irq(1);
    PIC_enable_irq(14);
    PIC_enable_irq(15);

    idt_pointer.limit = sizeof(idt) - 1;
    idt_pointer.address = idt;

    uint16 selector = 0x08; // kernel code selector
    uint8 zero = 0x00;
    uint8 type_attr = 0b10001110; // interrupt only available to kernel (not user code)

    for (int i = 0; i < 64; i++)
    {
        uint32 interrupt_handler_address = (uint32)Interrupts::AllInterruptHandlers[i];
        uint16 offset_0_15 = interrupt_handler_address & 0x0000FFFF;
        uint16 offset_16_31 = interrupt_handler_address >> 16;

        idt[i].offset_1 = offset_0_15;
        idt[i].selector = selector;
        idt[i].zero = zero;
        idt[i].type_attr = type_attr;
        idt[i].offset_2 = offset_16_31;
    }

    asm("lidt %0" ::"m"(idt_pointer)
        : "memory");

    asm("sti");
}

void PIC_sendEOI(uint8 irq)
{
    if (irq >= 8)
        Serial::OutB(PIC2_CMD, 0x20);

    Serial::OutB(PIC1_CMD, 0x20);
}

void PIC_remap(uint8 offset)
{
    Serial::OutB(PIC1_CMD, ICW1_INIT | ICW1_ICW4); // starts the initialization sequence (in cascade mode)
    Serial::OutB(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    Serial::OutB(PIC1_DATA, offset);     // ICW2: Master PIC vector offset
    Serial::OutB(PIC2_DATA, offset + 8); // ICW2: Slave PIC vector offset
    Serial::OutB(PIC1_DATA, 4);          // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    Serial::OutB(PIC2_DATA, 2);          // ICW3: tell Slave PIC its cascade identity (0000 0010)

    Serial::OutB(PIC1_DATA, ICW4_8086);
    Serial::OutB(PIC2_DATA, ICW4_8086);

    // Mask -- start out with all IRQs disabled.
    // https://github.com/SerenityOS/serenity/blob/master/Kernel/Interrupts/PIC.cpp
    Serial::OutB(PIC1_DATA, 0xff);
    Serial::OutB(PIC1_DATA, 0xff);

    // ...except IRQ2, since that's needed for the master to let through slave interrupts.
    PIC_enable_irq(2);
}

void PIC_disable_irq(uint8 irq)
{
    uint16 port;
    uint8 value;

    if (irq < 8)
    {
        port = PIC1_DATA;
    }
    else
    {
        port = PIC2_DATA;
        irq -= 8;
    }

    value = Serial::InB(port) | (1 << irq);
    Serial::OutB(port, value);
}

void PIC_enable_irq(uint8 irq)
{
    uint16 port;
    uint8 value;

    if (irq < 8)
    {
        port = PIC1_DATA;
    }
    else
    {
        port = PIC2_DATA;
        irq -= 8;
    }

    value = Serial::InB(port) & ~(1 << irq);
    Serial::OutB(port, value);
}

void Cpu::Panic()
{
    asm("int %0" ::"i"(ISR_KERNEL_PANIC));
}

void Cpu::PanicInternal()
{
    Debug::PrintStack(64);

    Terminal::WriteLine("  KERNEL PANIC  ", VGA_FG_RED | VGA_BG_WHITE);

    // Should really make this do something
    while (1)
        ;
}
