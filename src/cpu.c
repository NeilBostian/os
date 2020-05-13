#include "types.h"
#include "terminal.h"
#include "cpu.h"
#include "serial.h"

#define PIC_ISR_OFFSET 0x20
#define ISR_KEYBOARD PIC_ISR_OFFSET + 1
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

static gdt_entry gdt[5];
static gdt_descriptor gdt_pointer;

static idt_entry idt[64];
static idt_descriptor idt_pointer;

static void PIC_sendEOI(uint8 irq);
static void PIC_remap(uint8 offset);
static void PIC_disable_irq(uint8 irq);
static void PIC_enable_irq(uint8 irq);
static void panic_internal();

extern void reload_segments();
extern void isr_handle();

extern void interrupt_handler_0();
extern void interrupt_handler_1();
extern void interrupt_handler_2();
extern void interrupt_handler_3();
extern void interrupt_handler_4();
extern void interrupt_handler_5();
extern void interrupt_handler_6();
extern void interrupt_handler_7();
extern void interrupt_handler_8();
extern void interrupt_handler_9();
extern void interrupt_handler_10();
extern void interrupt_handler_11();
extern void interrupt_handler_12();
extern void interrupt_handler_13();
extern void interrupt_handler_14();
extern void interrupt_handler_15();
extern void interrupt_handler_16();
extern void interrupt_handler_17();
extern void interrupt_handler_18();
extern void interrupt_handler_19();
extern void interrupt_handler_20();
extern void interrupt_handler_21();
extern void interrupt_handler_22();
extern void interrupt_handler_23();
extern void interrupt_handler_24();
extern void interrupt_handler_25();
extern void interrupt_handler_26();
extern void interrupt_handler_27();
extern void interrupt_handler_28();
extern void interrupt_handler_29();
extern void interrupt_handler_30();
extern void interrupt_handler_31();
extern void interrupt_handler_32();
extern void interrupt_handler_33();
extern void interrupt_handler_34();
extern void interrupt_handler_35();
extern void interrupt_handler_36();
extern void interrupt_handler_37();
extern void interrupt_handler_38();
extern void interrupt_handler_39();
extern void interrupt_handler_40();
extern void interrupt_handler_41();
extern void interrupt_handler_42();
extern void interrupt_handler_43();
extern void interrupt_handler_44();
extern void interrupt_handler_45();
extern void interrupt_handler_46();
extern void interrupt_handler_47();
extern void interrupt_handler_48();
extern void interrupt_handler_49();
extern void interrupt_handler_50();
extern void interrupt_handler_51();
extern void interrupt_handler_52();
extern void interrupt_handler_53();
extern void interrupt_handler_54();
extern void interrupt_handler_55();
extern void interrupt_handler_56();
extern void interrupt_handler_57();
extern void interrupt_handler_58();
extern void interrupt_handler_59();
extern void interrupt_handler_60();
extern void interrupt_handler_61();
extern void interrupt_handler_62();
extern void interrupt_handler_63();

static void *interrupt_handlers[] = {
    (void *)interrupt_handler_0,
    (void *)interrupt_handler_1,
    (void *)interrupt_handler_2,
    (void *)interrupt_handler_3,
    (void *)interrupt_handler_4,
    (void *)interrupt_handler_5,
    (void *)interrupt_handler_6,
    (void *)interrupt_handler_7,
    (void *)interrupt_handler_8,
    (void *)interrupt_handler_9,
    (void *)interrupt_handler_10,
    (void *)interrupt_handler_11,
    (void *)interrupt_handler_12,
    (void *)interrupt_handler_13,
    (void *)interrupt_handler_14,
    (void *)interrupt_handler_15,
    (void *)interrupt_handler_16,
    (void *)interrupt_handler_17,
    (void *)interrupt_handler_18,
    (void *)interrupt_handler_19,
    (void *)interrupt_handler_20,
    (void *)interrupt_handler_21,
    (void *)interrupt_handler_22,
    (void *)interrupt_handler_23,
    (void *)interrupt_handler_24,
    (void *)interrupt_handler_25,
    (void *)interrupt_handler_26,
    (void *)interrupt_handler_27,
    (void *)interrupt_handler_28,
    (void *)interrupt_handler_29,
    (void *)interrupt_handler_30,
    (void *)interrupt_handler_31,
    (void *)interrupt_handler_32,
    (void *)interrupt_handler_33,
    (void *)interrupt_handler_34,
    (void *)interrupt_handler_35,
    (void *)interrupt_handler_36,
    (void *)interrupt_handler_37,
    (void *)interrupt_handler_38,
    (void *)interrupt_handler_39,
    (void *)interrupt_handler_40,
    (void *)interrupt_handler_41,
    (void *)interrupt_handler_42,
    (void *)interrupt_handler_43,
    (void *)interrupt_handler_44,
    (void *)interrupt_handler_45,
    (void *)interrupt_handler_46,
    (void *)interrupt_handler_47,
    (void *)interrupt_handler_48,
    (void *)interrupt_handler_49,
    (void *)interrupt_handler_50,
    (void *)interrupt_handler_51,
    (void *)interrupt_handler_52,
    (void *)interrupt_handler_53,
    (void *)interrupt_handler_54,
    (void *)interrupt_handler_55,
    (void *)interrupt_handler_56,
    (void *)interrupt_handler_57,
    (void *)interrupt_handler_58,
    (void *)interrupt_handler_59,
    (void *)interrupt_handler_60,
    (void *)interrupt_handler_61,
    (void *)interrupt_handler_62,
    (void *)interrupt_handler_63};

void create_gdt()
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

    reload_segments();
}

void interrupt_handler(cpu_state cpu, uint32 isr, uint32 error_code, uint32 eip)
{
    if (isr == ISR_KEYBOARD)
    {
        uint8 scan_code = inb(0x60);

        // terminal_write("kbd_scan=0x");
        // terminal_write_uint8(scan_code);
        // terminal_write("\n");

        switch (scan_code)
        {
        case 0x47:
            terminal_pagetop();
            break;
        case 0x48:
            terminal_pageup(1);
            break;
        case 0x49:
            terminal_pageup(10);
            break;
        case 0x4F:
            terminal_pagebottom();
            break;
        case 0x50:
            terminal_pagedown(1);
            break;
        case 0x51:
            terminal_pagedown(10);
            break;
        }
    }
    else if (isr == ISR_KERNEL_PANIC)
    {
        panic_internal();
    }
    else
    {
        terminal_write("Received interrupt 0x");
        terminal_write_uint8((uint8)isr);
        terminal_writeline(", no handler configured for this ISR.");
    }

    if (isr >= PIC_ISR_OFFSET && isr < PIC_ISR_OFFSET + 16)
    {
        PIC_sendEOI(isr - PIC_ISR_OFFSET);
    }
}

void create_idt()
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
        uint32 interrupt_handler_address = (uint32)interrupt_handlers[i];
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
        outb(PIC2_CMD, 0x20);

    outb(PIC1_CMD, 0x20);
}

void PIC_remap(uint8 offset)
{
    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4); // starts the initialization sequence (in cascade mode)
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    outb(PIC1_DATA, offset);     // ICW2: Master PIC vector offset
    outb(PIC2_DATA, offset + 8); // ICW2: Slave PIC vector offset
    outb(PIC1_DATA, 4);          // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outb(PIC2_DATA, 2);          // ICW3: tell Slave PIC its cascade identity (0000 0010)

    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    // Mask -- start out with all IRQs disabled.
    // https://github.com/SerenityOS/serenity/blob/master/Kernel/Interrupts/PIC.cpp
    outb(PIC1_DATA, 0xff);
    outb(PIC1_DATA, 0xff);

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

    value = inb(port) | (1 << irq);
    outb(port, value);
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

    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

void panic()
{
    asm("int %0" ::"i"(ISR_KERNEL_PANIC));
}

void panic_internal()
{
    // Should really make this do something
    while (1)
        ;
}