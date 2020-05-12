#include "types.h"
#include "terminal.h"
#include "cpu.h"

static gdt_entry gdt[5];
static gdt_descriptor gdt_pointer;

static idt_entry idt[64];
static idt_descriptor idt_pointer;

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

static uint32 interrupt_handlers[] = {
    (uint32)interrupt_handler_0,
    (uint32)interrupt_handler_1,
    (uint32)interrupt_handler_2,
    (uint32)interrupt_handler_3,
    (uint32)interrupt_handler_4,
    (uint32)interrupt_handler_5,
    (uint32)interrupt_handler_6,
    (uint32)interrupt_handler_7,
    (uint32)interrupt_handler_8,
    (uint32)interrupt_handler_9,
    (uint32)interrupt_handler_10,
    (uint32)interrupt_handler_11,
    (uint32)interrupt_handler_12,
    (uint32)interrupt_handler_13,
    (uint32)interrupt_handler_14,
    (uint32)interrupt_handler_15,
    (uint32)interrupt_handler_16,
    (uint32)interrupt_handler_17,
    (uint32)interrupt_handler_18,
    (uint32)interrupt_handler_19,
    (uint32)interrupt_handler_20,
    (uint32)interrupt_handler_21,
    (uint32)interrupt_handler_22,
    (uint32)interrupt_handler_23,
    (uint32)interrupt_handler_24,
    (uint32)interrupt_handler_25,
    (uint32)interrupt_handler_26,
    (uint32)interrupt_handler_27,
    (uint32)interrupt_handler_28,
    (uint32)interrupt_handler_29,
    (uint32)interrupt_handler_30,
    (uint32)interrupt_handler_31,
    (uint32)interrupt_handler_32,
    (uint32)interrupt_handler_33,
    (uint32)interrupt_handler_34,
    (uint32)interrupt_handler_35,
    (uint32)interrupt_handler_36,
    (uint32)interrupt_handler_37,
    (uint32)interrupt_handler_38,
    (uint32)interrupt_handler_39,
    (uint32)interrupt_handler_40,
    (uint32)interrupt_handler_41,
    (uint32)interrupt_handler_42,
    (uint32)interrupt_handler_43,
    (uint32)interrupt_handler_44,
    (uint32)interrupt_handler_45,
    (uint32)interrupt_handler_46,
    (uint32)interrupt_handler_47,
    (uint32)interrupt_handler_48,
    (uint32)interrupt_handler_49,
    (uint32)interrupt_handler_50,
    (uint32)interrupt_handler_51,
    (uint32)interrupt_handler_52,
    (uint32)interrupt_handler_53,
    (uint32)interrupt_handler_54,
    (uint32)interrupt_handler_55,
    (uint32)interrupt_handler_56,
    (uint32)interrupt_handler_57,
    (uint32)interrupt_handler_58,
    (uint32)interrupt_handler_59,
    (uint32)interrupt_handler_60,
    (uint32)interrupt_handler_61,
    (uint32)interrupt_handler_62,
    (uint32)interrupt_handler_63};

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
    terminal_writeint(isr);
}

void create_idt()
{
    idt_pointer.limit = sizeof(idt) - 1;
    idt_pointer.address = idt;

    uint16 selector = 0x08;
    uint8 zero = 0x00;
    uint8 type_attr = 0b10001110;

    for (int i = 0; i < 64; i++)
    {
        uint32 interrupt_handler_address = interrupt_handlers[i];
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
}